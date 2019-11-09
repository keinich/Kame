#pragma once

#include "DescriptorAllocation.h"

#include <d3d12.h>
#include <wrl.h>

#include <map>
#include <memory>
#include <mutex>
#include <queue>

namespace Kame {

  class DescriptorAllocatorPage : public std::enable_shared_from_this<DescriptorAllocatorPage> {

  public: // Functions

    DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

    D3D12_DESCRIPTOR_HEAP_TYPE GetHeapType() const;

    bool HasSpace(uint32_t numDescriptors) const;

    uint32_t GetNumFreeHandles() const;

    DescriptorAllocation Allocate(uint32_t numDescriptors);

    void Free(DescriptorAllocation&& descriptorHandle, uint64_t frameNumber);

    void ReleaseStaleDescriptors(uint64_t frameNumber);

  protected: // Functions

    uint32_t ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle);

    void AddNewBlock(uint32_t offset, uint32_t numDescriptors);

    void FreeBlock(uint32_t offset, uint32_t numDescriptors);

  private: // Fields

    using OffsetType = uint32_t;
    using SizeType = uint32_t;

    struct FreeBlockInfo;
    using FreeListByOffset = std::map < OffsetType, FreeBlockInfo>;
    using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

    struct FreeBlockInfo {
      FreeBlockInfo(SizeType size) {
        Size = size;
      }

      SizeType Size;
      FreeListBySize::iterator FreeListBySizeIterator;
    };

    struct StaleDescriptorInfo {
      StaleDescriptorInfo(OffsetType offset, SizeType size, uint64_t frame) {
        Offset = offset;
        Size = size;
        FrameNumber = frame;
      }
      OffsetType Offset;
      SizeType Size;
      uint64_t FrameNumber;
    };

    using StaleDescriptorQueue = std::queue<StaleDescriptorInfo>;

    FreeListByOffset _FreeListByOffset;
    FreeListBySize _FreeListBySize;
    StaleDescriptorQueue _StaleDescriptors;

    ComPtr<ID3D12DescriptorHeap> _D3D12DescriptorHeap;
    D3D12_DESCRIPTOR_HEAP_TYPE _HeapType;
    CD3DX12_CPU_DESCRIPTOR_HANDLE _BaseDescriptor;
    uint32_t _DescriptorHandleIncrementSize;
    uint32_t _NumDescriptorsInHeap;
    uint32_t _NumFreeHandles;

    std::mutex _AllocationMutex;
  };

}