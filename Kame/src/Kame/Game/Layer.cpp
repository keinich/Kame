#include "kmpch.h"
#include "Layer.h"
#include <Kame/Graphics/GraphicsCore.h>
#include <Kame/Graphics/RenderTarget.h>
#include "Game.h"

namespace Kame {

  Layer::Layer(
    Game* parent,
    float relativeLeft, float relativeTop, float relativeWidth, float relativeHeight,
    const std::string& name
  ) :
    _ScreenRectangle(
      parent->GetScreenRectangle(),
      relativeLeft, relativeTop, relativeWidth, relativeHeight
    ) {

    _Game = parent;
    _DebugName = name;

    _SceneRenderTarget.reset(GraphicsCore::CreateRenderTarget());
    Initialize();
  }

  Layer::~Layer() {}

  void Layer::OnResize(ResizeEventArgs& eventArgs) {
    _ScreenRectangle.UpdateAbsoluteValues(_Game->GetScreenRectangle());
    _SceneRenderTarget->Resize(_ScreenRectangle.GetAbsoluteWidth(), _ScreenRectangle.GetAbsoluteHeight());
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
    auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(
      hDRFormat,
      _ScreenRectangle.GetAbsoluteWidth(),
      _ScreenRectangle.GetAbsoluteHeight()
    );
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
    auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(
      depthBufferFormat,
      _ScreenRectangle.GetAbsoluteWidth(),
      _ScreenRectangle.GetAbsoluteHeight()
    );
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthClearValue;
    depthClearValue.Format = depthDesc.Format;
    depthClearValue.DepthStencil = { 1.0f, 0 };

    _SceneDepthTexture = GraphicsCore::CreateTexture(depthDesc, &depthClearValue, TextureUsage::Depth, L"Depth Render Target");
  }

  void ScreenRectangle::UpdateAbsoluteValues(
    UINT parentLeft, UINT parentTop, UINT64 parentWidth, UINT parentHeight
  ) {
    _AbsoluteLeft = parentLeft + _RelativeLeft * parentWidth;
    _AbsoluteTop = parentTop + _RelativeTop * parentHeight;
    _AbsoluteWidth = _RelativeWidth * parentWidth;
    _AbsoluteHeight = _RelativeHeight * parentHeight;
  }

  void ScreenRectangle::UpdateAbsoluteValues(ScreenRectangle& parentRectangle) {
    UpdateAbsoluteValues(
      parentRectangle.GetAbsoluteLeft(),
      parentRectangle.GetAbsoluteTop(),
      parentRectangle.GetAbsoluteWidth(),
      parentRectangle.GetAbsoluteHeight()
    );
  }

  UINT ScreenRectangle::GetAbsoluteLeft() {
    return _AbsoluteLeft;
  }

  UINT ScreenRectangle::GetAbsoluteTop() {
    return _AbsoluteTop;
  }

  UINT64 ScreenRectangle::GetAbsoluteWidth() const {
    return _AbsoluteWidth;
  }

  UINT ScreenRectangle::GetAbsoluteHeight() const {
    return _AbsoluteHeight;
  }

}
