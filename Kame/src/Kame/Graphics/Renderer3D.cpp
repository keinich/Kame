#include "kmpch.h"
#include "Renderer3D.h"
#include <Kame/Graphics/Mesh.h>
#include <Kame/Game/Objects/MeshComponent.h>
#include <Kame/Graphics/Camera.h>
#include <Kame/Graphics/RenderApi/Renderprogram.h>
#include <Kame/Graphics/RenderApi/RenderprogramSignature.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/Material.h>
#include <Kame/Graphics/Scene3D.h>
#include <Kame/Graphics/Light.h>
#include <Kame/Math/VectorMath.h>

//TODO
// Renderer3D is also a singleton (in the end it maybe doesnt have a state at all, beacause the render-programs are in the materials),
// Render-Method gets passed a scene (Meshes, Ligths, Skybox etc.) and a camera
// outside the Renderer sets the Viewport according to the Specifications of the rendering Layer
// question is: are there resources of the 3D-Render-Programs (Shadow-Stuff f.e.) that need to exist per Viewport-Dimension
// if so, they have to be passed to the renderer3d->Render
// the render target in the end is always the SceneRenderTarget of the Renderer

using namespace DirectX; //For the math

namespace Kame {



  void XM_CALLCONV ComputeMatrices1(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Matrices& mat) {
    mat.ModelMatrix = model;
    mat.ModelViewMatrix = model * view;
    mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
    mat.ModelViewProjectionMatrix = model * viewProjection;
  }

  void Renderer3D::RenderScene(
    std::vector<Reference<MeshComponent>>& meshes,
    Camera* camera,
    RenderProgram* renderProgram,
    CommandList* commandList,
    Texture* texture,
    MaterialInstanceBase* matInstace,
    Scene3D* scene
  ) {

    //commandList->SetGraphicsRootSignature(renderProgram->GetSignature());
    //commandList->SetRenderProgram(renderProgram);

    //commandList->SetGraphicsRootSignature(matInstace->GetMaterial()->GetSignature());
    //commandList->SetRenderProgram(matInstace->GetMaterial()->GetProgram());



    // Upload lights
    /*LightProperties lightProps;
    lightProps.NumPointLights = static_cast<uint32_t>(m_PointLights.size());
    lightProps.NumSpotLights = static_cast<uint32_t>(m_SpotLights.size());

    commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProps);
    commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_PointLights);
    commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_SpotLights);*/

    XMMATRIX viewMatrix = camera->get_ViewMatrix().GetXmMatrix();
    XMMATRIX viewProjectionMatrix = viewMatrix * camera->get_ProjectionMatrix().GetXmMatrix();

    LightProperties lightProps;
    lightProps.NumPointLights = static_cast<uint32_t>(scene->GetPointLights().size());
    lightProps.NumSpotLights = static_cast<uint32_t>(scene->GetSpotLights().size());

    CalculateLightPositions(scene, viewMatrix);

    RenderTree renderTree;
    renderTree.Build(scene->GetMeshComponents(), viewMatrix, viewProjectionMatrix);

    for (auto it = renderTree.SignatureTreeByIdentifier.begin(); it != renderTree.SignatureTreeByIdentifier.end(); it++) {
      commandList->SetGraphicsRootSignature(it->second.Signature);
      for (auto it2 = it->second.ProgramTreeByIdentifier.begin(); it2 != it->second.ProgramTreeByIdentifier.end(); it2++) {
        commandList->SetRenderProgram(it2->second.Program);

        commandList->SetGraphics32BitConstants(DefaultMaterialRootParameters::LightPropertiesCB1, lightProps);
        commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::PointLights1, scene->GetPointLights());
        commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::SpotLights1, scene->GetSpotLights());

        for (auto it3 = it2->second.InstancedMeshesByMesh.begin(); it3 != it2->second.InstancedMeshesByMesh.end(); it3++) {
          std::vector<InstanceData> instanceData = it3->second.InstanceData;
          Mesh* mesh = it3->second.Mesh;
          XMMATRIX worldMatrix = instanceData[0].ModelMatrix;

          Matrices matrices;
          ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

          commandList->SetGraphicsDynamicConstantBuffer(DefaultMaterialRootParameters::MatricesCB1, matrices);
          commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::InstanceData1, instanceData);         



          it3->second.Material->ApplyParameters(commandList);

          mesh->Draw(commandList, instanceData.size());
        }
      }
    }

    //for (Reference<MeshComponent> meshComponent : scene->GetMeshComponents()) {

    //  commandList->SetGraphicsRootSignature(meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature());
    //  commandList->SetRenderProgram(meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram());

    //  XMMATRIX worldMatrix = meshComponent->GetWorldMatrix();

    //  Mat matrices;
    //  ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    //  commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

    //  meshComponent->GetMaterialInstance()->ApplyParameters(commandList);

    //  meshComponent->GetMesh()->Draw(commandList);

    //}
  }

  void Renderer3D::CalculateLightPositions(Kame::Scene3D* scene, const DirectX::XMMATRIX& viewMatrix) {
    for (int i = 0; i < scene->GetPointLights().size(); ++i) {
      PointLight& l = scene->GetPointLights()[i];

      Kame::Math::Vector4 positionWS = XMLoadFloat4(&l.PositionWS);
      Kame::Math::Vector4 positionVS = Kame::Math::Vector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS.GetXmVector());

    }

    for (int i = 0; i < scene->GetSpotLights().size(); ++i) {
      SpotLight& l = scene->GetSpotLights()[i];

      Kame::Math::Vector4 positionWS = XMLoadFloat4(&l.PositionWS);
      Kame::Math::Vector4 positionVS = Kame::Math::Vector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS.GetXmVector());

      Kame::Math::Vector4 directionWS = XMLoadFloat4(&l.DirectionWS);
      Kame::Math::Vector4 directionVS = XMVector3Normalize(XMVector3TransformNormal(directionWS.GetXmVector(), viewMatrix));
      
      XMStoreFloat4(&l.DirectionVS, directionVS.GetXmVector());
    }

  }

  void RenderTree::Build(
    std::vector<Reference<MeshComponent>> meshComponents,
    XMMATRIX viewMatrix,
    XMMATRIX viewProjectionMatrix
  ) {
    for (auto meshComponent : meshComponents) {
      size_t signatureIdentifier = meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature()->GetIdentifier();
      std::map<size_t, RenderProgramSignatureTree>::iterator x = SignatureTreeByIdentifier.find(signatureIdentifier);
      if (x == SignatureTreeByIdentifier.end()) {
        SignatureTreeByIdentifier.insert(
          std::map<size_t, RenderProgramSignatureTree>::value_type(signatureIdentifier, RenderProgramSignatureTree())
        );
        SignatureTreeByIdentifier[signatureIdentifier].Signature = meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature();
      }

      size_t programIdentifier = meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram()->GetIdentifier();
      std::map<size_t, RenderProgramMeshes>::iterator y = SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.find(
        programIdentifier
      );
      if (
        y == SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.end()
        ) {
        SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.insert(
          std::map<size_t, RenderProgramMeshes>::value_type(programIdentifier, RenderProgramMeshes())
        );
        SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier[programIdentifier].Program = meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram();
      }
      auto& rpm = SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier[programIdentifier].InstancedMeshesByMesh;
      std::map<Mesh*, InstancedMesh>::iterator z = rpm.find(meshComponent->GetMesh());
      if (z == rpm.end()) {
        rpm.insert(std::map<Mesh*, InstancedMesh>::value_type(meshComponent->GetMesh(), InstancedMesh()));
      }
      rpm[meshComponent->GetMesh()].Mesh = meshComponent->GetMesh();
      InstanceData& instanceData = InstanceData();
      XMMATRIX worldMatrix = meshComponent->GetWorldMatrix();
      Matrices matrices;
      ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);
      instanceData.InverseTransposeModelViewMatrix = matrices.InverseTransposeModelViewMatrix;
      instanceData.ModelMatrix = matrices.ModelMatrix;
      instanceData.ModelViewMatrix = matrices.ModelViewMatrix;
      instanceData.ModelViewProjectionMatrix = matrices.ModelViewProjectionMatrix;
      rpm[meshComponent->GetMesh()].InstanceData.push_back(instanceData);
      rpm[meshComponent->GetMesh()].Material = meshComponent->GetMaterialInstance();
    }
  }

}