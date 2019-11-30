#pragma once

#include <d3d12.h> //TODO abstract away D3D12_CLEAR_FLAGS, D3D12_VIEWPORT, D3D12_RECT

namespace Kame {

  class Texture;
  class RenderTarget;
  class RenderProgram;

  class CommandList {

  public:

    virtual void ClearTexture(const Texture* texture, const float clearColor[4]) = 0;
    virtual void ClearDepthStencilTexture(const Texture* texture, D3D12_CLEAR_FLAGS clearFlags, float depth = 1.0f, uint8_t stencil = 0) = 0;

    virtual void SetRenderTarget(const RenderTarget& renderTarget) = 0;
    virtual void SetViewport(const D3D12_VIEWPORT& viewport) = 0;
    virtual void SetScissorRect(const D3D12_RECT& scissorRect) = 0;

    virtual void SetRenderProgram(const RenderProgram* renderProgram) = 0;
  };

}