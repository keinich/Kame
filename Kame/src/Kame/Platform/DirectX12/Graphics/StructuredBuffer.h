#pragma once

#include "GpuBuffer.h"
#include "ByteAddressBuffer.h"

namespace Kame {
   
  class StructuredBuffer : public GpuBuffer {
  public:
    StructuredBuffer(const std::wstring& name = L"");
    StructuredBuffer(
      const D3D12_RESOURCE_DESC& resDesc,
      size_t numElements, size_t elementSize,
      const std::wstring& name = L""
    );

    /**
    * Get the number of elements contained in this buffer.
    */
    virtual size_t GetNumElements() const {
      return _NumElements;
    }

    /**
    * Get the size in bytes of each element in this buffer.
    */
    virtual size_t GetElementSize() const {
      return _ElementSize;
    }

    /**
     * Create the views for the buffer resource.
     * Used by the CommandList when setting the buffer contents.
     */
    virtual void CreateViews(size_t numElements, size_t elementSize) override;

    /**
     * Get the SRV for a resource.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const {
      return _SRV.GetDescriptorHandle();
    }

    /**
     * Get the UAV for a (sub)resource.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override {
      // Buffers don't have subresources.
      return _UAV.GetDescriptorHandle();
    }

    const ByteAddressBuffer& GetCounterBuffer() const {
      return _CounterBuffer;
    }

  private:
    size_t _NumElements;
    size_t _ElementSize;

    DescriptorAllocation _SRV;
    DescriptorAllocation _UAV;

    // A buffer to store the internal counter for the structured buffer.
    ByteAddressBuffer _CounterBuffer;
  };

}
