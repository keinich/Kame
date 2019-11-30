#pragma once

#include <d3d12.h> //TODO abstract away D3D12_CLEAR_FLAGS

namespace Kame {

  class Texture;

  class CommandList {

  public:

    virtual void ClearTexture(const Texture* texture, const float clearColor[4]) = 0;
    virtual void ClearDepthStencilTexture(const Texture* texture, D3D12_CLEAR_FLAGS clearFlags, float depth = 1.0f, uint8_t stencil = 0) = 0;

  };

}