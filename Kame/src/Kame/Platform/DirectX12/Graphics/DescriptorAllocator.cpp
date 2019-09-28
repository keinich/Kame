#include "kmpch.h"
#include "DescriptorAllocator.h"

#include "DX12Core.h"

namespace Kame {

  std::mutex DescriptorAllocator::_AllocationMutex;
  std::vector<ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::_DescriptorHeapPool;

  D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(uint32_t count) {

    if (_CurrentHeap == nullptr || _RemainingFreeHandles < count) {
      _CurrentHeap = RequestNewHeap(_Type);
      _CurrentHandle = _CurrentHeap->GetCPUDescriptorHandleForHeapStart();
      _RemainingFreeHandles = _NumDescriptorsPerHeap;

      if (_DescriptorSize == 0)
        _DescriptorSize = DX12Core::GetDevice()->GetDescriptorHandleIncrementSize(_Type);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE result = _CurrentHandle;
    _CurrentHandle.ptr += (count * _DescriptorSize);
    _RemainingFreeHandles -= count;
    return result;

  }

  void DescriptorAllocator::DestroyAll() {
    _DescriptorHeapPool.clear();
  }

  ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {

    std::lock_guard<std::mutex> LockGuard(_AllocationMutex);

    D3D12_DESCRIPTOR_HEAP_DESC desc;
    desc.Type = type;
    desc.NumDescriptors = _NumDescriptorsPerHeap;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 1;

    ComPtr<ID3D12DescriptorHeap> heap;
    ThrowIfFailed(DX12Core::GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));
    _DescriptorHeapPool.emplace_back(heap);
    return heap.Get();
    
  }

}
