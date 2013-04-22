#ifndef _MODEL_H_
#define _MODEL_H_

#include <iostream>
#include <string>
#include <vector>

#include "Point.h"

class Model
{
  private:
    std::vector<Vertex> _vertexes;
    std::vector<Face> _faces;

    Vertex getFaceNormal(Vertex v1, Vertex v2, Vertex v3) const;

    static void halfEdgeLoad(Model &m);

  public:
    Model();
    ~Model();

    void draw() const;

    static Model load(std::string filename);

    friend std::ostream &operator<<(std::ostream &os, const Model &m);
};

#endif
