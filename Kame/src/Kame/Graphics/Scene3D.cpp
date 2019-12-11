#include "kmpch.h"
#include "Scene3D.h"
#include "Light.h"
#include "Skybox.h"

namespace Kame {

  Scene3D::~Scene3D() {
    _MeshComponents.clear();
    _PointLights.clear();
    _SpotLights.clear();
  }

  void Scene3D::AddMeshComponent(Reference<MeshComponent> meshComponent) {
    _MeshComponents.push_back(meshComponent);
  }

  void Scene3D::AddPointLight(PointLight pointLight) {
    _PointLights.push_back(pointLight);
  }

  void Scene3D::AddSpotLight(SpotLight spotLight) {
    _SpotLights.push_back(spotLight);
  }

  void Scene3D::CreateSkybox(Texture* skyboxTexture) {
    _Skybox = CreateReference<Skybox>();
    _Skybox->SetTexture(skyboxTexture);
  }

}