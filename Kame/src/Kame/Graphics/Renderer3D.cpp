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

    // iterate Signatures
    for (
      auto signatureTree = renderTree.SignatureTreeByIdentifier.begin();
      signatureTree != renderTree.SignatureTreeByIdentifier.end();
      signatureTree++
      ) {
      commandList->SetGraphicsRootSignature(signatureTree->second.Signature);

      // iterate Programs
      for (
        auto programTree = signatureTree->second.ProgramTreeByIdentifier.begin();
        programTree != signatureTree->second.ProgramTreeByIdentifier.end();
        programTree++
        ) {
        commandList->SetRenderProgram(programTree->second.Program);

        commandList->SetGraphics32BitConstants(DefaultMaterialRootParameters::LightPropertiesCB1, lightProps);
        commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::PointLights1, scene->GetPointLights());
        commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::SpotLights1, scene->GetSpotLights());

        // iterate Meshes
        for (
          auto instancedMeshesByMaterial = programTree->second.InstancedMeshesByMaterialByMesh.begin();
          instancedMeshesByMaterial != programTree->second.InstancedMeshesByMaterialByMesh.end();
          instancedMeshesByMaterial++
          ) {

          // iterate Materials
          for (
            auto instancedMeshes = instancedMeshesByMaterial->second.begin();
            instancedMeshes != instancedMeshesByMaterial->second.end();
            instancedMeshes++
            ) {

            std::vector<InstanceData> instanceData = instancedMeshes->second.InstanceDataGroup.InstanceData;
            Mesh* mesh = instancedMeshes->second.Mesh;
            XMMATRIX worldMatrix = instanceData[0].ModelMatrix;

            Matrices matrices;
            ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

            commandList->SetGraphicsDynamicConstantBuffer(DefaultMaterialRootParameters::MatricesCB1, matrices);
            commandList->SetGraphicsDynamicStructuredBuffer(DefaultMaterialRootParameters::InstanceData1, instanceData);

            // There must be at least one materialInstance
            // wen can use the ApplyParameters of the first Instance since all the Instances share the type
            // ApplyParameters will only consider the static MaterialParameters
            instancedMeshes->second.InstanceDataGroup.MaterialInstances[0]->ApplyParameters(commandList);

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

  // TODO cache this tree and insert / delete /update
  void RenderTree::Build(
    std::vector<Reference<MeshComponent>> meshComponents,
    XMMATRIX viewMatrix,
    XMMATRIX viewProjectionMatrix
  ) {
    for (auto meshComponent : meshComponents) {
      RenderProgramSignature* signature = meshComponent->GetMaterialInstance()->GetMaterial()->GetSignature();
      size_t signatureIdentifier = signature->GetIdentifier();
      std::map<size_t, RenderProgramSignatureTree>::iterator signatureTree = SignatureTreeByIdentifier.find(signatureIdentifier);
      if (signatureTree == SignatureTreeByIdentifier.end()) {
        SignatureTreeByIdentifier.insert(
          std::map<size_t, RenderProgramSignatureTree>::value_type(signatureIdentifier, RenderProgramSignatureTree())
        );
        SignatureTreeByIdentifier[signatureIdentifier].Signature = signature;
      }
      RenderProgram* program = meshComponent->GetMaterialInstance()->GetMaterial()->GetProgram();
      size_t programIdentifier = program->GetIdentifier();
      std::map<size_t, RenderProgramMeshes>::iterator programTree = SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.find(
        programIdentifier
      );
      if (
        programTree == SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.end()
        ) {
        SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier.insert(
          std::map<size_t, RenderProgramMeshes>::value_type(programIdentifier, RenderProgramMeshes())
        );
        SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier[programIdentifier].Program = program;
      }
      auto& rpm = SignatureTreeByIdentifier[signatureIdentifier].ProgramTreeByIdentifier[programIdentifier].InstancedMeshesByMaterialByMesh;
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
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].Material = meshComponent->GetMaterialInstance()->GetMaterial();
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
      for (UINT i = 0; i < rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialInstances.size(); ++i) {
        MaterialInstanceBase* materialInstance = rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialInstances[i];
        if (materialInstance == meshComponent->GetMaterialInstance()) {
          // If the MaterialInstance is found, assign the index of it to the MaterialIndex of the Instance Data
          instanceData.MaterialParameterIndex = i;
          found = true;
          break;
        }
      }
      if (!found) {
        rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialInstances.push_back(meshComponent->GetMaterialInstance());
        instanceData.MaterialParameterIndex = rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.MaterialInstances.size() - 1;
      }
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].InstanceDataGroup.InstanceData.push_back(instanceData);
      rpm[meshComponent->GetMesh()][meshComponent->GetMaterialInstance()->GetMaterial()].Material = meshComponent->GetMaterialInstance()->GetMaterial();
    }
  }

}