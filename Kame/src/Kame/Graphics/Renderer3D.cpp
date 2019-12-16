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
#include <Kame/Graphics/Skybox.h>

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
    Camera* camera,
    CommandList* commandList,
    Scene3D* scene
  ) {

    XMMATRIX viewMatrix = camera->get_ViewMatrix().GetXmMatrix();
    XMMATRIX projMatrixSkybox = camera->get_ProjectionMatrix().GetXmMatrix();
    XMMATRIX viewProjectionMatrix = viewMatrix * projMatrixSkybox;

    Skybox* skybox = scene->GetSkybox();
    if (skybox) {
      auto viewMatrixSkybox = XMMatrixTranspose(XMMatrixRotationQuaternion(camera->get_Rotation().GetXmVector()));
      DirectX::XMMATRIX viewProjMatrixSkybox = viewMatrixSkybox * projMatrixSkybox;

      //commandList->SetPipelineState(m_SkyboxPipelineState);
      commandList->SetRenderProgram(skybox->GetProgram());
      commandList->SetGraphicsRootSignature(skybox->GetProgram()->GetSignature());

      commandList->SetGraphics32BitConstants(0, viewProjMatrixSkybox);

      skybox->Draw(commandList);
    }

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
          for (auto it4 = it3->second.begin(); it4 != it3->second.end(); it4++) {

            std::vector<InstanceData> instanceData = it4->second.InstanceDataGroup.InstanceData;
            Mesh* mesh = it4->second.Mesh;
            XMMATRIX worldMatrix = instanceData[0].ModelMatrix;

            Matrices matrices;
            ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

            commandList->SetGraphicsDynamicConstantBuffer(DefaultMaterialRootParameters::MatricesCB1, matrices);

            commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::InstanceData1, instanceData);

            //commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::MaterialParameters, it4->second.InstanceDataGroup.MaterialMap);
            it4->second.Material->ApplyParameters(commandList);

            mesh->Draw(commandList, instanceData.size());
          }
        }
      }
    }

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
      std::map<Mesh*, std::map<MaterialBase*, InstancedMesh>>::iterator instancedMeshesByMaterial = rpm.find(meshComponent->GetMesh());
      if (instancedMeshesByMaterial == rpm.end()) {
        rpm.insert(std::map < Mesh*, std::map<MaterialBase*, InstancedMesh >> ::value_type(meshComponent->GetMesh(), std::map < MaterialBase*, InstancedMesh>()));
      }
      std::map<MaterialBase*, InstancedMesh>::iterator instancedMeshes = rpm[meshComponent->GetMesh()].find(meshComponent->GetMaterialInstance()->GetMaterial());
      if (instancedMeshes == rpm[meshComponent->GetMesh()].end()) {
        rpm[meshComponent->GetMesh()].insert(
          std::map < MaterialBase*, InstancedMesh > ::value_type(meshComponent->GetMaterialInstance()->GetMaterial(), InstancedMesh()));
      }
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].Mesh = meshComponent->GetMesh();
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].Material = meshComponent->GetMaterialInstance();
      InstanceData& instanceData = InstanceData();
      XMMATRIX worldMatrix = meshComponent->GetWorldMatrix();
      Matrices matrices;
      ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);
      instanceData.InverseTransposeModelViewMatrix = matrices.InverseTransposeModelViewMatrix;
      instanceData.ModelMatrix = matrices.ModelMatrix;
      instanceData.ModelViewMatrix = matrices.ModelViewMatrix;
      instanceData.ModelViewProjectionMatrix = matrices.ModelViewProjectionMatrix;
      // Find the MaterialInstance in the MaterialMap of the InstanceGroup of the rpm
      bool found = false;
      for (UINT i = 0; i < rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialMap.size(); ++i) {
        MaterialInstanceBase* materialInstance = rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialMap[i];
        if (materialInstance == meshComponent->GetMaterialInstance()) {
          // If the MaterialInstance is found, assign the index of it to the MaterialIndex of the Instance Data
          instanceData.MaterialParameterIndex = i;
          found = true;
          break;
        }
      }
      if (!found) {
        rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialMap.push_back(meshComponent->GetMaterialInstance());
        instanceData.MaterialParameterIndex = rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialMap.size() - 1;
      }
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.InstanceData.push_back(instanceData);
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].Material = meshComponent->GetMaterialInstance();
    }
  }

}