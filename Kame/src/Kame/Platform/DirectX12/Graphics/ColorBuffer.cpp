#include "kmpch.h"
#include "ColorBuffer.h"

#include "DX12Core.h"

namespace Kame {

  void ColorBuffer::CreateFromSwapChain(const std::wstring& name, ID3D12Resource* baseResource, D3D12_CPU_DESCRIPTOR_HANDLE rtv) {

    AssociateWithResource(DX12Core::GetDevice(), name, baseResource, D3D12_RESOURCE_STATE_PRESENT);

    DX12Core::GetDevice()->CreateRenderTargetView(_Resource, nullptr, rtv);

  }

}