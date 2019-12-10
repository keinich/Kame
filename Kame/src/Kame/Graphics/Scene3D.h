#pragma once
#include <vector>

#include <Kame/Core/References.h>

#include <Kame/Graphics/Light.h>

namespace Kame {

  class MeshComponent;
  class PointLight;
  class SpotLight;

  class Scene3D {

  public:
    Scene3D() {}
    virtual ~Scene3D();

    void AddMeshComponent(Reference<MeshComponent> meshComponent);
    void AddPointLight(PointLight pointLight);
    void AddSpotLight(SpotLight spotLight);


    const inline std::vector<Reference<MeshComponent>>& GetMeshComponents() { return _MeshComponents; }
    inline std::vector<PointLight>& GetPointLights() { return _PointLights; }
    inline std::vector<SpotLight>& GetSpotLights() { return _SpotLights; }

  private:
    std::vector<Reference<MeshComponent>> _MeshComponents;
    std::vector<PointLight> _PointLights;
    std::vector<SpotLight> _SpotLights;
  };

}