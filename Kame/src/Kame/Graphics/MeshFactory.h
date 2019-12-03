#pragma once

#include <memory>
#include "Mesh.h"

namespace Kame {

  class MeshFactory {
  public:
    static Mesh* GetCube(float size = 1, bool rhcoords = false);

  protected:
    static void CreateCube(Mesh* mesh, float size = 1, bool rhcoords = false);

  };

}