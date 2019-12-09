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

//TODO
// Renderer3D is also a singleton (in the end it maybe doesnt have a state at all, beacause the render-programs are in the materials),
// Render-Method gets passed a scene (Meshes, Ligths, Skybox etc.) and a camera
// outside the Renderer sets the Viewport according to the Specifications of the rendering Layer
// question is: are there resources of the 3D-Render-Programs (Shadow-Stuff f.e.) that need to exist per Viewport-Dimension
// if so, they have to be passed to the renderer3d->Render
// the render target in the end is always the SceneRenderTarget of the Renderer

using namespace DirectX; //For the math

namespace Kame {

  struct Mat {
    XMMATRIX ModelMatrix;
    XMMATRIX ModelViewMatrix;
    XMMATRIX InverseTransposeModelViewMatrix;
    XMMATRIX ModelViewProjectionMatrix;
  };

  void XM_CALLCONV ComputeMatrices1(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Mat& mat) {
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

    RenderTree renderTree;
    renderTree.Build(scene->GetMeshComponents());

    for (auto it = renderTree.RenderProgramMeshesBySignatureIdentifier.begin(); it != renderTree.RenderProgramMeshesBySignatureIdentifier.end(); it++) {
      commandList->SetGraphicsRootSignature(it->second.Signature);
      for (auto it2 = it->second.RenderProgramMeshesByProgramIdentifier.begin(); it2 != it->second.RenderProgramMeshesByProgramIdentifier.end(); it2++) {
        commandList->SetRenderProgram(it2->second.Program);
        for (auto meshComponent : it2->second.MeshComponents) {
          XMMATRIX worldMatrix = meshComponent->GetWorldMatrix();

          Mat matrices;
          ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

          commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);

          meshComponent->GetMaterialInstance()->ApplyParameters(commandList);

          meshComponent->GetMesh()->Draw(commandList);
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

  void RenderTree::Build(std::vector<Reference<MeshComponent>> meshComponents) {
    for (auto meshComponent : meshComponents) {
      size_t signatureIdentifier = meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature()->GetIdentifier();
      std::map<size_t, RenderProgramSignatureTree>::iterator x = RenderProgramMeshesBySignatureIdentifier.find(signatureIdentifier);
      if (x == RenderProgramMeshesBySignatureIdentifier.end()) {
        RenderProgramMeshesBySignatureIdentifier.insert(
          std::map<size_t, RenderProgramSignatureTree>::value_type(signatureIdentifier, RenderProgramSignatureTree())
        );
        RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].Signature = meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature();
      }

      size_t programIdentifier = meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram()->GetIdentifier();
      std::map<size_t, RenderProgramMeshes>::iterator y = RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].RenderProgramMeshesByProgramIdentifier.find(programIdentifier);
      if (
        y == RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].RenderProgramMeshesByProgramIdentifier.end()
        ) {
        RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].RenderProgramMeshesByProgramIdentifier.insert(
          std::map<size_t, RenderProgramMeshes>::value_type(programIdentifier, RenderProgramMeshes())
        );
        RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].RenderProgramMeshesByProgramIdentifier[programIdentifier].Program = meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram();
      }
      RenderProgramMeshesBySignatureIdentifier[signatureIdentifier].RenderProgramMeshesByProgramIdentifier[programIdentifier].MeshComponents.push_back(meshComponent.get());
    }
  }

}