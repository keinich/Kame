#pragma once

#include <d3d.h> //TODO abstract away DXGI_FORMAT
#include "GpuResource.h"
#include "TextureUsage.h"
#include <Kame/Core/References.h>

namespace Kame {

  class Texture : public GpuResource {

  public:

    virtual bool IsValid1() const = 0;

    virtual void LoadFromFile(const std::wstring& fileName, TextureUsage textureUsage) = 0;
    virtual void Create(size_t Width, size_t Height, DXGI_FORMAT Format, const void* InitialData) = 0;

    virtual UINT64 GetWidth() const = 0;
    virtual UINT64 GetHeight() const = 0;

    virtual DXGI_FORMAT GetFormat() const = 0;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1) = 0;

    virtual Reference<Texture> ToCubeMap(UINT width, std::wstring name) = 0;

  };

}