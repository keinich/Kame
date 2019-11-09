#include "kmpch.h"
#include "DescriptorAllocator_3dgep.h"

#include "DescriptorAllocatorPage.h"

namespace Kame {

  DescriptorAllocator_3dgep::DescriptorAllocator_3dgep(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptorsPerHeap) {
    _HeapType = type;
    _NumDescriptorsPerHeap = numDescriptorsPerHeap;
  }

  DescriptorAllocator_3dgep::~DescriptorAllocator_3dgep() {}

  DescriptorAllocation DescriptorAllocator_3dgep::Allocate(uint32_t numDescriptors) {

    std::lock_guard<std::mutex> lock(_AllocationMutex);

    DescriptorAllocation allocation;

    for (auto iter = _AvalibaleHeaps.begin(); iter != _AvalibaleHeaps.end(); ++iter) {
      auto allocatorPage = _HeapPool[*iter];

      allocation = allocatorPage->Allocate(numDescriptors);

      if (allocatorPage->GetNumFreeHandles() == 0) {
        iter = _AvalibaleHeaps.erase(iter);
      }

      if (!allocation.IsNull()) {
        break;
      }
    }

    if (allocation.IsNull()) {
      _NumDescriptorsPerHeap = std::max(_NumDescriptorsPerHeap, numDescriptors);
      auto newPage = CreateAllocatorPage();
      allocation = newPage->Allocate(numDescriptors);
    }

    return allocation;

  }

  void DescriptorAllocator_3dgep::ReleaseStaleDescriptors(uint64_t frameNumber) {
    std::lock_guard < std::mutex> lock(_AllocationMutex);

    for (size_t i = 0; i < _HeapPool.size(); ++i) {
      auto page = _HeapPool[i];

      page->ReleaseStaleDescriptors(frameNumber);

      if (page->GetNumFreeHandles() > 0) {
        _AvalibaleHeaps.insert(i);
      }
    }
  }

  std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocator_3dgep::CreateAllocatorPage() {
    auto newPage = std::make_shared<DescriptorAllocatorPage>(_HeapType, _NumDescriptorsPerHeap);

    _HeapPool.emplace_back(newPage);
    _AvalibaleHeaps.insert(_HeapPool.size() - 1);

    return newPage;
  }

}