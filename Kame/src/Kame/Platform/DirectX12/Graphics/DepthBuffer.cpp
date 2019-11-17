#include "kmpch.h"
//#include "DepthBuffer.h"
//#include "DX12Core.h"
//
//void Kame::DepthBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr) {
//
//  D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
//    DXGI_FORMAT_D32_FLOAT,
//    width, height,
//    1, 0, 1, 0,
//    D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
//  );
//
//  D3D12_CLEAR_VALUE optimizedClearValue = {};
//  optimizedClearValue.Format = format;
//  optimizedClearValue.DepthStencil = { 1.0f, 0 };
//
//  CreateTextureResource(name, resourceDesc, optimizedClearValue, VidMemPtr);
//  CreateDerivedViews(format);
//}
//
//void Kame::DepthBuffer::CreateDerivedViews(DXGI_FORMAT format) {
//
//  ID3D12Resource* resource = _Resource1.Get();
//  ID3D12Device* device = DX12Core::GetDevice();
//
//  // DSV
//
//  D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//  dsvDesc.Format = GetDsvFormat(DXGI_FORMAT_D32_FLOAT);
//  if (resource->GetDesc().SampleDesc.Count == 1) {
//    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//    dsvDesc.Texture2D.MipSlice = 0;
//  }
//  else {
//    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
//  }
//
//  if (_cpuDescriptorHandles[0].ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN) {
//    _cpuDescriptorHandles[0] = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//    _cpuDescriptorHandles[1] = DX12Core::Get()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//  }
//
//  dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
//  device->CreateDepthStencilView(
//    resource,
//    &dsvDesc,
//    _cpuDescriptorHandles[0]
//  );
//
//  // TODO Mini-Engine calls CreateDsv 1 - 3 more times with different flags -> ???
//
//}
