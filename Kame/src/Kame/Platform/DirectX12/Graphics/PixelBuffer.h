#pragma once

#include "GpuResource.h"

namespace Kame {

  class PixelBuffer : public GpuResource {

  public:

    PixelBuffer() :
      _Width(0),
      _Height(0),
      _ArraySize(0),
      _Format(DXGI_FORMAT_UNKNOWN) {
    }

  protected: // Functions

    void AssociateWithResource(ID3D12Device* device, const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

    void CreateTextureResource(
      const std::wstring& name, 
      const D3D12_RESOURCE_DESC& resourceDesc, 
      D3D12_CLEAR_VALUE clearValue, 
      D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
    );

    static DXGI_FORMAT GetDsvFormat(DXGI_FORMAT format);

  protected: // Fields

    uint32_t _Width;
    uint32_t _Height;
    uint32_t _ArraySize;
    DXGI_FORMAT _Format;

  };

}