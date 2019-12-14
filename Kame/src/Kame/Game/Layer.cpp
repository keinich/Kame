#include "kmpch.h"
#include "Layer.h"
#include <Kame/Graphics/GraphicsCore.h>
#include <Kame/Graphics/RenderTarget.h>

namespace Kame {

  Layer::Layer(UINT64 width, UINT height, const std::string& name) {
    _DebugName = name;
    _Width = width;
    _Height = height;
    _SceneRenderTarget.reset(GraphicsCore::CreateRenderTarget());
    Initialize();
  }

  Layer::~Layer() {}

  void Layer::OnResize(ResizeEventArgs& eventArgs) {
    _Width = eventArgs.Width;
    _Height = eventArgs.Height;
    _SceneRenderTarget->Resize(_Width, _Height);
  }

  void Layer::Initialize() {
    CreateSceneTexture();
    CreateSceneDepthTexture();    
    _SceneRenderTarget->AttachTexture(AttachmentPoint::Color0, _SceneTexture.get());
    _SceneRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, _SceneDepthTexture.get());
  }

  void Layer::CreateSceneTexture() {
    DXGI_FORMAT hDRFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    // Create an off-screen render target with a single color buffer and a depth buffer.
    auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(hDRFormat, _Width, _Height);
    colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE colorClearValue;
    colorClearValue.Format = colorDesc.Format;
    colorClearValue.Color[0] = 0.4f;
    colorClearValue.Color[1] = 0.6f;
    colorClearValue.Color[2] = 0.9f;
    colorClearValue.Color[3] = 1.0f;

    _SceneTexture = GraphicsCore::CreateTexture(
      colorDesc, &colorClearValue,
      TextureUsage::RenderTarget,
      L"HDR Texture"
    );

  }

  void Layer::CreateSceneDepthTexture() {
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
    auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, _Width, _Height);
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthClearValue;
    depthClearValue.Format = depthDesc.Format;
    depthClearValue.DepthStencil = { 1.0f, 0 };

    _SceneDepthTexture = GraphicsCore::CreateTexture(depthDesc, &depthClearValue, TextureUsage::Depth, L"Depth Render Target");
  }

}
