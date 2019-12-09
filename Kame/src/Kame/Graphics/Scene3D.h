#pragma once
#include <vector>

#include <Kame/Core/References.h>

namespace Kame {

  class MeshComponent;

  class Scene3D {

  public:
    Scene3D() {}
    virtual ~Scene3D();

    void AddMeshComponent(Reference<MeshComponent> meshComponent);

    const inline std::vector<Reference<MeshComponent>>& GetMeshComponents() { return _MeshComponents; }

  private:
    std::vector<Reference<MeshComponent>> _MeshComponents;

  };

}