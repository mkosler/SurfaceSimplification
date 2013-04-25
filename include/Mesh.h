#ifndef _MESH_H_
#define _MESH_H_

#include <algorithm>
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
    struct HalfEdge;
    struct Edge;
    struct Vertex;
    struct Face;

    // All half edge structs are based on slide code
    struct HalfEdge
    {
      HalfEdge *next, *prev, *flip;
      Vertex *origin; // The vertex at the base of the ray cast in the direction of the half edge
      Face *face;
      Edge *edge; // The non-oriented edge
    };

    struct Vertex
    {
      Point<3> position; // 3D coordinates
      Point<5> QEF; // Quadratic Error Function

      HalfEdge *edge; // Any random half edge touching the vertex; since I can reach them all, it doesn't matter which one
    };

    struct Face
    {
      Point<3> indexes; // Store the index information for easy original edge creation

      HalfEdge *edge; // Again, any random half edge will do
    };

    struct Edge
    {
      Point<5> combineQEF;
      bool removable; // A stupidly simple lazy removal flag for the priority queue operations

      HalfEdge *halfEdge; // Either side is fine
    };

    // Used for the STL heap operations;
    // I am having a bit of trouble understanding what from the QEF
    // I should be comparing. I made an "educated" guess that it is
    // the last element in the QEF, but I doubt that is correct.
    struct HeapCompare
    {
      bool operator()(Edge *e1, Edge *e2) const
      {
        return e1->combineQEF[4] < e2->combineQEF[4];
      }
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

    static Mesh load(const char *filename);
};

#endif
