#ifndef _MESH_H_
#define _MESH_H_

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Point.h"

class Mesh
{
  public:
    struct Edge;
    struct Vertex;
    struct Face;

    struct HalfEdge
    {
      HalfEdge *next, *prev, *flip;
      Vertex *origin;
      Face *face;
      Edge *edge;
    };

    struct Vertex
    {
      Point<3> position;
      Point<5> QEF;
      HalfEdge *edge;
    };

    struct Face
    {
      Point<3> indexes;
      HalfEdge *edge;
    };

    struct Edge
    {
      HalfEdge *halfEdge;
    };

  private:
    std::vector<Vertex*> _vertexes;
    std::vector<Face*> _faces;
    std::vector<Edge*> _edges;
    std::vector<HalfEdge*> _halfEdges;

    Point<3> getNormal(Point<3> v1, Point<3> v2, Point<3> v3) const;

  public:
    Mesh();
    ~Mesh();

    void draw() const;
    void simplify(const size_t faces);

    Face *getFace(size_t i);

    static Mesh load(const char *filename);
};

#endif
