#pragma once

#include "kmpch.h"

namespace Kame {

  struct VertexPosColor {
    Vector3 Position;
    Vector3 Color;
    VertexPosColor(Vector3 pos, Vector3 col) :
      Position(pos),
      Color(col) {}

  };

  class KAME_API Mesh {

  public:
    Mesh();
    Mesh(VertexPosColor* vertices, WORD* indices);
    ~Mesh();

    static Mesh* CreateCube();

  private: // Fields
    VertexPosColor* _Vertices;
    WORD* _Indices;
  };
}