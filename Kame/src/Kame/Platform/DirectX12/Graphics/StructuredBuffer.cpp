#include "kmpch.h"
#include "StructuredBuffer.h"

#include "DX12Core.h"
#include "ResourceStateTracker.h"
#include "dxd12.h"

namespace Kame {

  StructuredBuffer::StructuredBuffer(const std::wstring& name)
    : GpuBuffer(name)
    , _CounterBuffer(
      CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
      1, 4, name + L" Counter"
    )
    , _NumElements(0)
    , _ElementSize(0) {
    _SRV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _UAV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  }

  StructuredBuffer::StructuredBuffer(const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name)
    : GpuBuffer(resDesc, numElements, elementSize, name)
    , _CounterBuffer(
      CD3DX12_RESOURCE_DESC::Buffer(4, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
      1, 4, name + L" Counter")
    , _NumElements(numElements)
    , _ElementSize(elementSize) {
    _SRV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _UAV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  }

  void StructuredBuffer::CreateViews(size_t numElements, size_t elementSize) {
    auto device = DX12Core::Get()->GetDevice();

    _NumElements = numElements;
    _ElementSize = elementSize;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = static_cast<UINT>(_NumElements);
    srvDesc.Buffer.StructureByteStride = static_cast<UINT>(_ElementSize);
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    device->CreateShaderResourceView(_Resource1.Get(),
      &srvDesc,
      _SRV.GetDescriptorHandle());

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.Buffer.CounterOffsetInBytes = 0;
    uavDesc.Buffer.NumElements = static_cast<UINT>(_NumElements);
    uavDesc.Buffer.StructureByteStride = static_cast<UINT>(_ElementSize);
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    device->CreateUnorderedAccessView(_Resource1.Get(),
      _CounterBuffer.GetD3D12Resource().Get(),
      &uavDesc,
      _UAV.GetDescriptorHandle());
  }
  
}