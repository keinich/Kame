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
  class Mesh;

  enum DefaultMaterialRootParameters {
    MatricesCB1,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB1,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB1,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    InstanceData1,        // StructuredBuffer<InstanceData> g_InstanceData : register (t0, space1);
    PointLights1,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights1,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures1,           // Texture2D DiffuseTexture : register( t2 );
    NumRootParameters1
  };

  struct LightProperties {
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
  };

  struct InstanceData {
    DirectX::XMMATRIX ModelMatrix;
    DirectX::XMMATRIX ModelViewMatrix;
    DirectX::XMMATRIX InverseTransposeModelViewMatrix;
    DirectX::XMMATRIX ModelViewProjectionMatrix;
  };

  struct InstancedMesh {
    Mesh* Mesh;
    std::vector<InstanceData> InstanceData;
    MaterialInstanceBase* Material;
  };

  struct RenderProgramMeshes {
    RenderProgram* Program;
    std::map<Mesh*, InstancedMesh> InstancedMeshesByMesh;
    //std::vector<MeshComponent*> MeshComponents;
  };

  struct Matrices {
    DirectX::XMMATRIX ModelMatrix;
    DirectX::XMMATRIX ModelViewMatrix;
    DirectX::XMMATRIX InverseTransposeModelViewMatrix;
    DirectX::XMMATRIX ModelViewProjectionMatrix;
  };

  struct RenderProgramSignatureTree {
    RenderProgramSignatureTree() {}
    virtual ~RenderProgramSignatureTree() {}
    RenderProgramSignature* Signature;
    std::map<size_t, RenderProgramMeshes> ProgramTreeByIdentifier;
  };

  struct RenderTree {
    void Build(
      std::vector<Reference<MeshComponent>> meshComponents,
      DirectX::XMMATRIX viewMatrix,
      DirectX::XMMATRIX viewProjectionMatrix
    );
    std::map<size_t, RenderProgramSignatureTree> SignatureTreeByIdentifier;
  };

  class Renderer3D {

  public:

    //inline static Renderer3D* Get() { return _Instance; }

    static void RenderScene(
      Camera* camera,
      CommandList* commandList,
      Scene3D* scene
    );

    static void CalculateLightPositions(Kame::Scene3D* scene, const DirectX::XMMATRIX& viewMatrix);

  private:

    //static Renderer3D* _Instance;
    //Renderer3D();
    //virtual ~Renderer3D();

  };

}