#pragma once

#include <memory>
#include "Mesh.h"

namespace Kame {

  class MeshFactory {

  public:
    static Mesh* GetCube(float size = 1, bool rhcoords = false);
    static Mesh* GetSphere(float diameter = 1, size_t tessellation = 16, bool rhcoords = false);
    static Mesh* GetCone(float diameter = 1, float height = 1, size_t tessellation = 32, bool rhcoords = false);
    static Mesh* GetTorus(float diameter = 1, float thickness = 0.333f, size_t tessellation = 32, bool rhcoords = false);
    static Mesh* GetPlane(float width = 1, float height = 1, bool rhcoords = false);
    static Mesh* GetDebugCube(float size = 1, bool rhcoords = false);

  protected:
    static void CreateCube(Mesh* mesh, float size = 1, bool rhcoords = false);
    static void CreateSphere(Mesh* mesh, float diameter , size_t tessellation , bool rhcoords );
    static void CreateCone(Mesh* mesh, float diameter , float height , size_t tessellation , bool rhcoords );
    static void CreateTorus(Mesh* mesh, float diameter , float thickness , size_t tessellation , bool rhcoords );
    static void CreatePlane(Mesh* mesh, float width , float height , bool rhcoords );
    static void CreateDebugCube(Mesh* mesh, float size , bool rhcoords );

  };

}