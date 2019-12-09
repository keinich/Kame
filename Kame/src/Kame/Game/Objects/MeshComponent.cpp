#include "kmpch.h"
#include "MeshComponent.h"

namespace Kame {

  Kame::MeshComponent::MeshComponent() :
    _Mesh(nullptr),
    _MaterialInstance(nullptr) {
    _WorldMatrix = DirectX::XMMatrixIdentity();
  }

}

