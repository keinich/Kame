#pragma once

#include <map>

namespace Kame {

  class Mesh;

  class MeshManager {

  public:
    
    static void Create();
    static void Destroy();

    static Mesh* GetMesh(std::wstring identifier);

  protected: // Methods

    static MeshManager* Get();

  protected: // Fields

    static MeshManager* s_Instance;

    std::map<std::wstring, std::unique_ptr<Mesh>> _MeshByIdentifier;

  };

}