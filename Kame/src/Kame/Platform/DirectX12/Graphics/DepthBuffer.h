//#pragma once
//
//#include "PixelBuffer.h"
//
//namespace Kame {
//
//  class DepthBuffer : public PixelBuffer {
//
//  public: // Functions
//
//    DepthBuffer() {
//      _cpuDescriptorHandles[0].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
//      _cpuDescriptorHandles[1].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
//      _cpuDescriptorHandles[2].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
//      _cpuDescriptorHandles[3].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
//    }
//
//    void Create(
//      const std::wstring& name,
//      uint32_t width, uint32_t height,
//      DXGI_FORMAT format,
//      D3D12_GPU_VIRTUAL_ADDRESS VidMemPtr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN
//    );
//
//    const D3D12_CPU_DESCRIPTOR_HANDLE& GetDsv() const { return _cpuDescriptorHandles[0]; }
//
//  private: // Functions
//
//    void CreateDerivedViews(DXGI_FORMAT format);
//
//  private: // Fields
//
//    D3D12_CPU_DESCRIPTOR_HANDLE _cpuDescriptorHandles[4];
//
//  };
//
//}