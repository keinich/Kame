#pragma once

#include <map>
#include "Mesh.h"

namespace Kame {

  class ManagedMesh : public Mesh {
  public:
    ManagedMesh(const std::wstring& identifier) :
      _Identifier(identifier) {}

  private:
    std::wstring _Identifier;
  };

  class MeshManager {

    friend class MeshFactory;

  public:

    static void Create();
    static void Destroy();

    virtual ~MeshManager();


  protected: // Methods

    static MeshManager* Get();
    static std::pair<ManagedMesh*, bool> GetOrCreateMesh(std::wstring identifier);

  protected: // Fields

    static MeshManager* s_Instance;

    std::map<std::wstring, NotCopyableReference<ManagedMesh>> _MeshByIdentifier;

  };

}