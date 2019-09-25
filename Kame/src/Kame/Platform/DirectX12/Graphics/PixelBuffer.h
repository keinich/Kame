#pragma once

#include "GpuResource.h"

namespace Kame {

  class PixelBuffer : public GpuResource {

  public:
    
    PixelBuffer() {}

  protected: // Functions

    void AssociateWithResource(ID3D12Device* device, const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

  protected: // Fields

    uint32_t _Width;
    uint32_t _Height;
    uint32_t _ArraySize;
    DXGI_FORMAT _Format;   

  };

}