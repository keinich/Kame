#pragma once

namespace Kame {

  class DescriptorAllocator {

  public:

    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) :
      _Type(type),
      _CurrentHeap(nullptr) {}

    ~DescriptorAllocator() {

    }
    D3D12_CPU_DESCRIPTOR_HANDLE Allocate(uint32_t count);

  protected: // Functions


  protected: // Fields    

    D3D12_DESCRIPTOR_HEAP_TYPE _Type;
    ID3D12DescriptorHeap* _CurrentHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE _CurrentHandle;
    uint32_t _DescriptorSize;
    uint32_t _RemainingFreeHandles;

  };

  class GlobalDescriptorAllocator {

    friend class DescriptorAllocator;

  public:

    static GlobalDescriptorAllocator* Get() {
      if (_Instance == nullptr)
        _Instance = new  GlobalDescriptorAllocator();
      return _Instance;
    }

    ~GlobalDescriptorAllocator() {}

    void Shutdown() { _DescriptorHeapPool.clear(); }

  protected: // Functions

    ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

  protected: // Fields

    const uint32_t _NumDescriptorsPerHeap = 256;
    std::mutex _AllocationMutex;
    static GlobalDescriptorAllocator* _Instance;
    std::vector<ComPtr<ID3D12DescriptorHeap>> _DescriptorHeapPool;

  };

}