#pragma once

#include <vector>
#include <map>

#include <Kame/Core/References.h>
#include <Kame/Graphics/RenderApi/Texture.h>

namespace Kame {

  class MeshComponent;
  class Camera;
  class RenderProgramSignature;
  class RenderProgram;
  class CommandList;
  class MaterialInstanceBase;
  class Scene3D;

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

  struct RenderProgramMeshes {
    RenderProgram* Program;
    std::vector<MeshComponent*> MeshComponents;
  };

  struct RenderProgramSignatureTree {
    RenderProgramSignatureTree() {}
    virtual ~RenderProgramSignatureTree() {}
    RenderProgramSignature* Signature;
    std::map<size_t, RenderProgramMeshes> RenderProgramMeshesByProgramIdentifier;
  };

  struct RenderTree {
    void Build(std::vector<Reference<MeshComponent>> meshComponents);
    std::map<size_t, RenderProgramSignatureTree> RenderProgramMeshesBySignatureIdentifier;
  };

  class Renderer3D {

  public:

    //inline static Renderer3D* Get() { return _Instance; }

    static void RenderScene(
      std::vector<Reference<MeshComponent>>& meshes,
      Camera* camera,
      RenderProgram* renderProgram,
      CommandList* commandList,
      Texture* texture,
      MaterialInstanceBase* matInstace,
      Scene3D* scene
    );

  private:

    //static Renderer3D* _Instance;
    //Renderer3D();
    //virtual ~Renderer3D();

  };

}