#pragma once

namespace Kame {

  class DescriptorAllocator {

  public:

    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) :
      _Type(type),
      _CurrentHeap(nullptr) {
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t count);

    static void DestroyAll();

  protected: // Functions

    static ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

  protected: // Fields

    static const uint32_t _NumDescriptorsPerHeap = 256;
    static std::mutex _AllocationMutex;
    static std::vector<ComPtr<ID3D12DescriptorHeap>> _DescriptorHeapPool;

    D3D12_DESCRIPTOR_HEAP_TYPE _Type;
    ID3D12DescriptorHeap* _CurrentHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE _CurrentHandle;
    uint32_t _DescriptorSize;
    uint32_t _RemainingFreeHandles;

  };

}