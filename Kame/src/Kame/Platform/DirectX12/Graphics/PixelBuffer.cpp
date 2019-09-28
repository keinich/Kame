#include "kmpch.h"
#include "PixelBuffer.h"

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

}