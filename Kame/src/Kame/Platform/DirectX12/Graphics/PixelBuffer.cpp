#include "kmpch.h"
#include "PixelBuffer.h"
#include "DX12Core.h"

namespace Kame {

  void PixelBuffer::AssociateWithResource(ID3D12Device* device, const std::wstring& name, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState) {

    assert(resource != nullptr);
    D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();

    //_Resource = resource;
    _Resource1.Attach(resource);
    _UsageState = currentState;

    _Width = (uint32_t)resourceDesc.Width;
    _Height = resourceDesc.Height;
    _ArraySize = resourceDesc.DepthOrArraySize;
    _Format = resourceDesc.Format;

#ifdef KAME_DEBUG
    //_Resource->SetName(name.c_str());
#endif

  }

  void PixelBuffer::CreateTextureResource(
    const std::wstring& name,
    const D3D12_RESOURCE_DESC& resourceDesc,
    D3D12_CLEAR_VALUE clearValue,
    D3D12_GPU_VIRTUAL_ADDRESS vidMemPtr
  ) {

    Destroy();

    auto device = DX12Core::GetDevice();
    ThrowIfFailed(device->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
      D3D12_HEAP_FLAG_NONE,
      &resourceDesc,
      D3D12_RESOURCE_STATE_DEPTH_WRITE, // TODO in Mini-Engine this gets initialized with STATE_COMMON (later transitioned as needed)
      &clearValue,
      IID_PPV_ARGS(&_Resource1)
    ));

    _UsageState = D3D12_RESOURCE_STATE_DEPTH_WRITE; // TODO in Mini-Engine this gets initialized with STATE_COMMON (later transitioned as needed)

#ifndef RELEASE
    _Resource1->SetName(name.c_str());
#else
#endif

  }

  DXGI_FORMAT PixelBuffer::GetDsvFormat(DXGI_FORMAT format) {
    switch (format) {
      // 32-bit Z w/ Stencil
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
      return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

      // No Stencil
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
      return DXGI_FORMAT_D32_FLOAT;

      // 24-bit Z
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
      return DXGI_FORMAT_D24_UNORM_S8_UINT;

      // 16-bit Z w/o Stencil
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
      return DXGI_FORMAT_D16_UNORM;

    default:
      return format;
    }
  }

}