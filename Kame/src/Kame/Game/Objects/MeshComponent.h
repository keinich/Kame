#pragma once

#include <Kame/Core/References.h>
#include <Kame/Math/Matrix4x4.h>

namespace Kame {

  class Mesh;
  class MaterialInstanceBase;

  class MeshComponent {

  public:
    MeshComponent();
    virtual ~MeshComponent() {};

    inline void SetMesh(Mesh* mesh) { _Mesh = mesh; }
    inline Mesh* GetMesh() { return _Mesh; }

    inline void SetMaterialInstance(Reference<MaterialInstanceBase> material) { _MaterialInstance = material; }
    inline MaterialInstanceBase* GetMaterialInstance() { return _MaterialInstance.get(); }

    inline void SetWorldMatirx(DirectX::XMMATRIX matrix) { _WorldMatrix = matrix; }
    inline DirectX::XMMATRIX& GetWorldMatrix() { return _WorldMatrix; }

  private:
    Mesh* _Mesh;
    Reference<MaterialInstanceBase> _MaterialInstance;
    DirectX::XMMATRIX _WorldMatrix;

  };

}