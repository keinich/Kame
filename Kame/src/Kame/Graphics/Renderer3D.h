#pragma once

#include <vector>

#include <Kame/Core/References.h>
#include <Kame/Graphics/RenderApi/Texture.h>

namespace Kame {

  class Mesh;
  class Camera;
  class RenderProgram;
  class CommandList;
  class MaterialInstanceBase;

  enum RootParameters {
    MatricesCB,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    PointLights,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures,           // Texture2D DiffuseTexture : register( t2 );
    NumRootParameters
  };

  struct LightProperties {
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
  };

  class Renderer3D {

  public:

    //inline static Renderer3D* Get() { return _Instance; }

    static void RenderScene(
      std::vector<Mesh*>& meshes, 
      Camera* camera, 
      RenderProgram* renderProgram, 
      CommandList* commandList, 
      Texture* texture,
      MaterialInstanceBase* matInstace
    );

  private:

    //static Renderer3D* _Instance;
    //Renderer3D();
    //virtual ~Renderer3D();

  };

}