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

    inline void SetMaterial(MaterialInstanceBase* material) { _Material = material; }
    inline MaterialInstanceBase* GetMaterial() { return _Material; }

    inline void SetWorldMatirx(DirectX::XMMATRIX matrix) { _WorldMatrix = matrix; }
    inline DirectX::XMMATRIX& GetWorldMatrix() { return _WorldMatrix; }

  private:
    Mesh* _Mesh;
    MaterialInstanceBase* _Material;
    DirectX::XMMATRIX _WorldMatrix;

  };

}