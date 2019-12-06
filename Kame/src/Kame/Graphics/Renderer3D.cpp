#include "kmpch.h"
#include "Renderer3D.h"
#include <Kame/Graphics/Mesh.h>
#include <Kame/Graphics/Camera.h>
#include <Kame/Graphics/RenderApi/Renderprogram.h>
#include <Kame/Graphics/RenderApi/RenderprogramSignature.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/Material.h>

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
    std::vector<Mesh*>& meshes,
    Camera* camera,
    RenderProgram* renderProgram,
    CommandList* commandList,
    Texture* texture
  ) {

    commandList->SetGraphicsRootSignature(renderProgram->GetSignature());
    commandList->SetRenderProgram(renderProgram);

    // Upload lights
    /*LightProperties lightProps;
    lightProps.NumPointLights = static_cast<uint32_t>(m_PointLights.size());
    lightProps.NumSpotLights = static_cast<uint32_t>(m_SpotLights.size());

    commandList->SetGraphics32BitConstants(RootParameters::LightPropertiesCB, lightProps);
    commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLights, m_PointLights);
    commandList->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLights, m_SpotLights);*/

    XMMATRIX viewMatrix = camera->get_ViewMatrix().GetXmMatrix();
    XMMATRIX viewProjectionMatrix = viewMatrix * camera->get_ProjectionMatrix().GetXmMatrix();

    for (Mesh* mesh : meshes) {

      // Draw the earth sphere
      XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
      XMMATRIX rotationMatrix = XMMatrixIdentity();
      XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
      XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

      Mat matrices;
      ComputeMatrices1(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCB, matrices);
      commandList->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCB, Material::White);
      commandList->SetShaderResourceViewTexture(RootParameters::Textures, 0, texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

      mesh->Draw(commandList);

    }
  }

}