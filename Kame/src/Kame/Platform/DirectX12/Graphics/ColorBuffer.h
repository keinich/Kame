#pragma once

#include "PixelBuffer.h"

namespace Kame {

  class ColorBuffer : public PixelBuffer {

  public:
    ColorBuffer() {
      _RtvHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    }

    // TODO remove rtv
    void CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource, D3D12_CPU_DESCRIPTOR_HANDLE rtv);

    const D3D12_CPU_DESCRIPTOR_HANDLE GetRtv() const { return _RtvHandle; };

  protected:

    D3D12_CPU_DESCRIPTOR_HANDLE _RtvHandle;

  };

}