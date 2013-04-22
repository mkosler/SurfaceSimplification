#ifndef _HALFEDGE_H_
#define _HALFEDGE_H_

struct HEEdge
{
  HEVert *vertex; // vertex at the end of the half-edge
  HEEdge *pair;   // oppositely oriented adjacent half-edge
  HEFace *face;   // face the half-edge borders
  HEEdge *next;   // next half-edge around the face
};

struct HEVert
{
  float x, y, z;
  HEEdge *edge; // one of the half-edges emaniating from the vertex
};

struct HEFace
{
  HEEdge *edge; // one of the half-edges bordering the face
};

#endif
