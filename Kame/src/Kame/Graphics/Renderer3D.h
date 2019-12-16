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
  class MaterialBase;
  class Scene3D;
  class Mesh; 

  struct LightProperties {
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
  };

  struct InstanceData {
    DirectX::XMMATRIX ModelMatrix;
    DirectX::XMMATRIX ModelViewMatrix;
    DirectX::XMMATRIX InverseTransposeModelViewMatrix;
    DirectX::XMMATRIX ModelViewProjectionMatrix;
    UINT MaterialParameterIndex;
    UINT InstancePad0;
    UINT InstancePad1;
    UINT InstancePad2;

  };

  struct InstanceDataGroup {
    std::vector<InstanceData> InstanceData;
    std::vector<MaterialInstanceBase*> MaterialMap;
  };

  struct InstancedMesh {
    Mesh* Mesh;
    InstanceDataGroup InstanceDataGroup;
    MaterialInstanceBase* Material;
  };

  struct RenderProgramMeshes {
    RenderProgram* Program;
    std::map<Mesh*, std::map<MaterialBase*, InstancedMesh>> InstancedMeshesByMesh;
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