#include "kmpch.h"
#include "DescriptorAllocator.h"

#include "DX12Core.h"

namespace Kame {

  GlobalDescriptorAllocator* GlobalDescriptorAllocator::_Instance = nullptr;

  D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(uint32_t count) {

    if (_CurrentHeap == nullptr || _RemainingFreeHandles < count) {      
      _CurrentHeap = DX12Core::GetGlobalDescriptorAllocator()->RequestNewHeap(_Type);
      _CurrentHandle = _CurrentHeap->GetCPUDescriptorHandleForHeapStart();
      _RemainingFreeHandles = DX12Core::GetGlobalDescriptorAllocator()->_NumDescriptorsPerHeap;

      if (_DescriptorSize == 0)
        _DescriptorSize = DX12Core::GetDevice()->GetDescriptorHandleIncrementSize(_Type);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE result = _CurrentHandle;
    _CurrentHandle.ptr += (count * _DescriptorSize);
    _RemainingFreeHandles -= count;
    return result;

  }

  ID3D12DescriptorHeap* GlobalDescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {

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