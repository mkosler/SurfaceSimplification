#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

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

Mesh Mesh::load(const char *filename)
{
  Mesh m;
  
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
        throw std::runtime_error("Unknown flag for simple .obj: " + flag);
        break;
    }
  }

  ifs.close();

  std::map<std::pair<size_t, size_t>, Edge*> edgeMap;
  for (size_t i = 0; i < m._faces.size(); i++) {
    // Grab a face
    Face *f = m._faces[i];

    // Walk around the face vertexes
    for (size_t j = 0; j < 3; j++) {
      // Create half-edge for each vertex pair
      // (this ends up being two half-edges for each pair)
      std::auto_ptr<Edge> e(new Edge());

      // Set the vertex to be the next in the walk
      e->vert = m._vertexes[f->indexes[(j + 1) % 3] - 1];

      // Set the vertexes edge pointer back to the current edge
      // Since we only need one edge to point to, this being
      // overwritten doesn't really matter
      e->vert->edge = e.get();

      // Set the face to be the current face
      e->face = f;

      // See if there already exists a half-edge for this vertex pair
      // if so, set the pair value for each
      // otherwise, add the edge to the map for easy lookup
      std::pair<size_t, size_t> p =
        std::make_pair(std::min(f->indexes[j], f->indexes[(j + 1) % 3]),
                       std::max(f->indexes[j], f->indexes[(j + 1) % 3]));
      std::map<std::pair<size_t, size_t>, Edge*>::iterator it =
        edgeMap.find(p);
      if (it == edgeMap.end()) {
        //edgeMap[p] = m._edges.size() + 1;
        edgeMap[p] = e.get();
      } else {
        e->pair = edgeMap[p];
        edgeMap[p]->pair = e.get();
        //e->pair = m._edges[edgeMap[p]];
        //m._edges[edgeMap[p]]->pair = e.get();
      }

      m._edges.push_back(e.release());
    }

    // Finish up the half-edges by setting their next
    Edge *e1 = m._edges[m._edges.size() - 3];
    Edge *e2 = m._edges[m._edges.size() - 2];
    Edge *e3 = m._edges[m._edges.size() - 1];

    e1->next = e2;
    e2->next = e3;
    e3->next = e1;

    // Pick an edge at random, you'll reach all of the other edges
    // no matter what
    f->edge = e1;
  }

  return m;
}
