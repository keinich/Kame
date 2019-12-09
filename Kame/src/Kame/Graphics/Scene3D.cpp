#include "kmpch.h"
#include "Scene3D.h"

namespace Kame {

  Scene3D::~Scene3D() {
    _MeshComponents.clear();
  }

  void Scene3D::AddMeshComponent(Reference<MeshComponent> meshComponent) {
    _MeshComponents.push_back(meshComponent);
  }

}