#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
  // Clean up all the freaking pointers
  while (!_vertexes.empty()) {
    delete _vertexes.back();
    _vertexes.pop_back();
  }

  while (!_faces.empty()) {
    delete _faces.back();
    _faces.pop_back();
  }

  while (!_edges.empty()) {
    delete _edges.back();
    _edges.pop_back();
  }

  while (!_halfEdges.empty()) {
    delete _halfEdges.back();
    _halfEdges.pop_back();
  }
}

// For lighting
Point<3> Mesh::getNormal(Point<3> v1, Point<3> v2, Point<3> v3) const
{
  Point<3> vec1 = v3 - v2;
  Point<3> vec2 = v1 - v2;

  Point<3> normal;
  normal[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
  normal[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
  normal[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

  normal.normalize();

  return normal;
}

void Mesh::draw() const
{
  for (size_t i = 0; i < _faces.size(); i++) {
    Face *f = _faces[i];

    Point<3> pts[3] = {
      _vertexes[f->indexes[0] - 1]->position,
      _vertexes[f->indexes[1] - 1]->position,
      _vertexes[f->indexes[2] - 1]->position,
    };

    Point<3> normal = getNormal(pts[0], pts[1], pts[2]);
    glNormal3f(normal[0], normal[1], normal[2]);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
      glVertex3f(pts[0][0], pts[0][1], pts[0][2]);
      glVertex3f(pts[1][0], pts[1][1], pts[1][2]);
      glVertex3f(pts[2][0], pts[2][1], pts[2][2]);
    glEnd();
  }
}

// The bread and butter
void Mesh::simplify(const size_t faces)
{
  // Build the QEF for each vertex
  for (size_t i = 0; i < _vertexes.size(); i++) {
    Vertex *v = _vertexes[i];

    // I guess this is what I get for missing the day we talked about this stuff
    // I made some "educated" guesses as to what our simplified QEF was refering to
    HalfEdge *he = v->edge;
    do {
      // Valence
      v->QEF[0]++;

      // Sum of surrounding vertexes
      v->QEF[1] += he->flip->origin->position[0];
      v->QEF[2] += he->flip->origin->position[1];
      v->QEF[3] += he->flip->origin->position[2];

      // Sum of vertex transpose times vertex
      v->QEF[4] += (he->flip->origin->position[0] * he->flip->origin->position[0]) +
                   (he->flip->origin->position[0] * he->flip->origin->position[0]) +
                   (he->flip->origin->position[0] * he->flip->origin->position[0]);

      he = he->flip->next;
    } while (he != v->edge);
  }

  std::vector<Edge*> heap;

  for (size_t i = 0; i < _edges.size(); i++) {
    Edge *e = _edges[i];
    Vertex *lv = e->halfEdge->origin, *rv = e->halfEdge->flip->origin;

    // From the assignment page, the combined QEF is simple addition
    for (size_t j = 0; j < 5; j++) {
      e->combineQEF[j] = lv->QEF[j] + rv->QEF[j];
    }

    heap.push_back(e);
  }

  // In hopes of having logarithmic priority queue operations, I used STL's make_heap to heapify
  // the vector
  std::make_heap(heap.begin(), heap.end(), HeapCompare());

  // Here is where I realized it was 5:30 AM on Thursday, April 25, 2013 and I had no real idea
  // if much of the above was even correct. It's also when I decided to start commenting the
  // heck out of my code.

  // This is where the while loop should start for reducing my current polygon count to the desired count

  // Pop the smallest error edge (skipping lazy deletes)
  std::pop_heap(heap.begin(), heap.end(), HeapCompare());
  Edge *e = heap.back(); heap.pop_back();

  // Here is the attempt at remembering what you said about lazy deletion; that is to say, I remember talking about
  // it, but my mind must have been elsewhere. So, I simply added a flag to each edge stating whether or not
  // I should ignore it (if it happened to be the minimum QEF)
  while (e->removable) {
    //std::vector<Edge*>::iterator it = std::find(_edges.begin(), _edges.end(), e);

    std::pop_heap(heap.begin(), heap.end(), HeapCompare());
    e = heap.back(); heap.pop_back();
  }

  // Here's the start of creating the next vertex
  std::auto_ptr<Vertex> nv(new Vertex());
  nv->QEF = e->combineQEF;

  HalfEdge *lhe = e->halfEdge, *rhe = e->halfEdge->flip;

  // Flag all of the edges surrounding each removed edge vertex as also removable
  // (for the lazy removal)
  do {
    lhe->edge->removable = true;
    lhe = lhe->flip->next;
  } while (lhe != e->halfEdge);

  do {
    rhe->edge->removable = true;
    rhe = rhe->flip->next;
  } while (rhe != e->halfEdge->flip);
}

Mesh Mesh::load(const char *filename)
{
  Mesh m;

  // The reading from the file is the same as before, but with 3 vertexes per face instead of 4
  // (That's why those teeth were so ugly, by the way. I was referencing a mysterious 4th
  // vertex, that just ended up being 0 for all faces
  std::ifstream ifs(filename);
  if (!ifs) {
    throw std::runtime_error("Unable to open file: " + std::string(filename));
  }

  char flag;
  std::string line;

  while (std::getline(ifs, line)) {
    std::istringstream iss(line);

    if (!(iss >> flag)) {
      break;
    }

    switch (flag) {
      case '#':
        break;
      case 'v':
        {
          std::auto_ptr<Vertex> v(new Vertex());
          iss >> v->position[0] >> v->position[1] >> v->position[2];
          m._vertexes.push_back(v.release());
        }
        break;
      case 'f':
        {
          std::auto_ptr<Face> f(new Face());
          iss >> f->indexes[0] >> f->indexes[1] >> f->indexes[2];
          m._faces.push_back(f.release());
        }
        break;
      default:
        throw std::runtime_error("Unknown flag for file: " + flag);
        break;
    }
  }

  ifs.close();

  // Now for some extra fun with half edges
  std::map<std::pair<size_t, size_t>, Edge*> edgeMap;
  for (size_t i = 0; i < m._faces.size(); i++) {
    // Grab a face
    Face *f = m._faces[i];

    // Walk around the vertexes, creating half edges for each full edge
    for (size_t j = 0; j < 3; j++) {
      std::auto_ptr<HalfEdge> he(new HalfEdge());

      // The current face created this half edge,
      // so it must be on the same side of the edge
      he->face = f;

      // The origin is the jth vertex (not the (j + 1)th
      he->origin = m._vertexes[f->indexes[j] - 1];

      // Since this half edge uses this vertex as its origin,
      // it is "pointing away" (irrelevant anyways)
      he->origin->edge = he.get();

      // Grab any of the half edges created;
      // Doesn't matter which, since I can reach all face half edges
      // from any single face half edge
      f->edge = he.get();

      // See if the "full" edge has been created yet
      std::pair<size_t, size_t> p =
        std::make_pair(std::min(f->indexes[j], f->indexes[(j + 1) % 3]),
                       std::max(f->indexes[j], f->indexes[(j + 1) % 3]));
      std::map<std::pair<size_t, size_t>, Edge*>::iterator it = edgeMap.find(p);

      // If not, create it, set this half edge as its reference,
      // and vice versa;
      // otherwise, use the found edge to grab the flip for both adjacent
      // half edges
      if (it == edgeMap.end()) {
        std::auto_ptr<Edge> e(new Edge());

        e->halfEdge = he.get();
        he->edge = e.get();

        edgeMap[p] = e.get();

        m._edges.push_back(e.release());
      } else {
        Edge *e = it->second;

        he->edge = e;

        he->flip = e->halfEdge;
        he->flip->flip = he.get();
      }

      // Add this half edge to the vector
      m._halfEdges.push_back(he.release());
    }

    // Grab the last three half edges to set the next and prev pointers
    HalfEdge *hes[3] = {
      m._halfEdges[m._halfEdges.size() - 3],
      m._halfEdges[m._halfEdges.size() - 2],
      m._halfEdges[m._halfEdges.size() - 1],
    };

    for (size_t j = 0; j < 3; j++) {
      hes[j]->next = hes[(j + 1) % 3];
      hes[j]->prev = hes[(j - 1) % 3];
    }
  }

  return m;
}
