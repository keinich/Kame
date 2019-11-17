#include "kmpch.h"
#include "ByteAddressBuffer.h"

#include "DX12Core.h"

#include "Kame/Math/MathCommon.h"

namespace Kame {

  ByteAddressBuffer::ByteAddressBuffer(const std::wstring& name)
    : GpuBuffer(name) {
    _SRV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _UAV = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  }

  ByteAddressBuffer::ByteAddressBuffer(const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name)
    : GpuBuffer(resDesc, numElements, elementSize, name) {}

  void ByteAddressBuffer::CreateViews(size_t numElements, size_t elementSize) {
    auto device = DX12Core::Get()->GetDevice();

    // Make sure buffer size is aligned to 4 bytes.
    _BufferSize = Math::AlignUp(numElements * elementSize, 4);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Buffer.NumElements = (UINT)_BufferSize / 4;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

    device->CreateShaderResourceView(_Resource1.Get(), &srvDesc, _SRV.GetDescriptorHandle());

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.Buffer.NumElements = (UINT)_BufferSize / 4;
    uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

    device->CreateUnorderedAccessView(_Resource1.Get(), nullptr, &uavDesc, _UAV.GetDescriptorHandle());
  }


}