#pragma once


#include <d3d12.h>

#include <cstdint>
#include <memory>

namespace Kame {

  class DescriptorAllocatorPage;

  class DescriptorAllocation {

  public: // Functions

    DescriptorAllocation();

    DescriptorAllocation(
      D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle,
      uint32_t numHandles,
      uint32_t descriptorSize,
      std::shared_ptr<DescriptorAllocatorPage> page
    );

    ~DescriptorAllocation();

    // Copies are not allowed
    DescriptorAllocation(const DescriptorAllocation&) = delete;
    DescriptorAllocation& operator=(const DescriptorAllocation&) = delete;

    // Move is allowed
    DescriptorAllocation(DescriptorAllocation&& allocation) noexcept;
    DescriptorAllocation& operator = (DescriptorAllocation&& other)  noexcept;

    bool IsNull() const;

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(uint32_t offset = 0) const;

    uint32_t GetNumHandles() const;

    std::shared_ptr<DescriptorAllocatorPage> GetDescriptorAllocatorPage() const;

  private: // Functions

    void Free();

    D3D12_CPU_DESCRIPTOR_HANDLE _Descriptor;
    uint32_t _NumHandles;
    uint32_t _DescriptorSize;

    std::shared_ptr<DescriptorAllocatorPage> _Page;
  };

}
