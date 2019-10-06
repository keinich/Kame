#include "kmpch.h"
#include "Mesh.h"

namespace Kame {

  Mesh::Mesh() {
    _Vertices = nullptr;
    _Indices = nullptr;
  }

  Mesh::Mesh(VertexPosColor* vertices, WORD* indices) {
    _Vertices = vertices;
    _Indices = indices;
  }

  Mesh::~Mesh() {
    delete[] _Vertices;
    _Vertices = nullptr;
    delete[] _Indices;
    _Indices = nullptr;
  }

  Mesh* Mesh::CreateCube() {
    VertexPosColor vertices[8] = {
        VertexPosColor(Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f)) , // 0
        VertexPosColor(Vector3(-1.0f,  1.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)), // 1
        VertexPosColor(Vector3(1.0f,  1.0f, -1.0f), Vector3(1.0f, 1.0f, 0.0f)), // 2
        VertexPosColor(Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 0.0f, 0.0f)) , // 3
        VertexPosColor(Vector3(-1.0f, -1.0f,  1.0f), Vector3(0.0f, 0.0f, 1.0f)) , // 4
        VertexPosColor(Vector3(-1.0f,  1.0f,  1.0f), Vector3(0.0f, 1.0f, 1.0f)) , // 5
        VertexPosColor(Vector3(1.0f,  1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f)) , // 6
        VertexPosColor(Vector3(1.0f, -1.0f,  1.0f), Vector3(1.0f, 0.0f, 1.0f)) // 7
    };
    WORD indices[36] = {
      0, 1, 2, 0, 2, 3,
      4, 6, 5, 4, 7, 6,
      4, 5, 1, 4, 1, 0,
      3, 2, 6, 3, 6, 7,
      1, 5, 6, 1, 6, 2,
      4, 0, 3, 4, 3, 7
    };
    Mesh* result = new Mesh(vertices, indices);
    return result;
  }

}