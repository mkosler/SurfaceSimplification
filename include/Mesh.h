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

    struct Edge
    {
      Vertex *vert;
      Edge *pair;
      Face *face;
      Edge *next;
    };

    struct Vertex
    {
      Point<3> position;
      Edge *edge;
    };

    struct Face
    {
      Point<3> indexes;
      Edge *edge;
    };

  private:
    std::vector<Vertex*> _vertexes;
    std::vector<Face*> _faces;
    std::vector<Edge*> _edges;

    Point<3> getNormal(Point<3> v1, Point<3> v2, Point<3> v3) const;

  public:
    Mesh();
    ~Mesh();

    void draw() const;

    static Mesh load(const char *filename);
};

#endif
