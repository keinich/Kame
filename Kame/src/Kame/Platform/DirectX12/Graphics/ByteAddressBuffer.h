#pragma once

#include "GpuBuffer.h"
#include "DescriptorAllocation.h"

#include "dxd12.h"

namespace Kame {

  class ByteAddressBuffer : public GpuBuffer {
  public:
    ByteAddressBuffer(const std::wstring& name = L"");
    ByteAddressBuffer(
      const D3D12_RESOURCE_DESC& resDesc,
      size_t numElements, size_t elementSize,
      const std::wstring& name = L""
    );

    size_t GetBufferSize() const {
      return _BufferSize;
    }

    /**
     * Create the views for the buffer resource.
     * Used by the CommandList when setting the buffer contents.
     */
    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    /**
     * Get the SRV for a resource.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override {
      return _SRV.GetDescriptorHandle();
    }

    /**
     * Get the UAV for a (sub)resource.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override {
      // Buffers only have a single subresource.
      return _UAV.GetDescriptorHandle();
    }

  protected:

  private:
    size_t _BufferSize;

    DescriptorAllocation _SRV;
    DescriptorAllocation _UAV;
  };

}