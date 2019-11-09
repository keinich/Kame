#pragma once

#include "DescriptorAllocation.h"

#include "dxd12.h"

#include <cstdint>
#include <mutex>
#include <memory>
#include <set>
#include <vector>


namespace Kame {

  class DescriptorAllocatorPage;

  class DescriptorAllocator_3dgep {

  public:

    DescriptorAllocator_3dgep(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap = 256);
    virtual ~DescriptorAllocator_3dgep();

    DescriptorAllocation Allocate(uint32_t numDescriptors = 1);

    void ReleaseStaleDescriptors(uint64_t frameNumber);


  private: // Fields
    
    using DescriptorHeapPool = std::vector<std::shared_ptr<DescriptorAllocatorPage>>;

    D3D12_DESCRIPTOR_HEAP_TYPE _HeapType;
    uint32_t _NumDescriptorsPerHeap;

    DescriptorHeapPool _HeapPool;
    std::set<size_t> _AvalibaleHeaps;

    std::mutex _AllocationMutex;

  private: // Functions

    std::shared_ptr<DescriptorAllocatorPage> CreateAllocatorPage();
  };

}