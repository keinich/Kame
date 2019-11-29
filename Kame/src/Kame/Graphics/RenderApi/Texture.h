#pragma once

#include <d3d.h> //TODO abstract away DXGI_FORMAT

namespace Kame {

  class Texture {

  public:

    virtual bool IsValid1() const = 0;

    virtual UINT64 GetWidth() const = 0;
    virtual UINT64 GetHeight() const = 0;

    virtual DXGI_FORMAT GetFormat() const = 0;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1) = 0;

  };

}