#pragma once

#include "PixelBuffer.h"

namespace Kame {

  class ColorBuffer : public PixelBuffer {

  public:
    ColorBuffer() {}

    // TODO remove rtv
    void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource, D3D12_CPU_DESCRIPTOR_HANDLE rtv);

  protected:

    D3D12_CPU_DESCRIPTOR_HANDLE _RtvHandle;

  };

}