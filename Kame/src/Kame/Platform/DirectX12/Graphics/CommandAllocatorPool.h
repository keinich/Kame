#pragma once

#include "kmpch.h"

namespace Kame {


  class CommandAllocatorPool {
    
  public:
    CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type);
    ~CommandAllocatorPool();

    void Create(ID3D12Device* device);
    void Shutdown();

    ID3D12CommandAllocator* RequestAllocator(uint64_t completedFenceValue);
    void DiscardAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

  private: // Fields

    const D3D12_COMMAND_LIST_TYPE _CommandListType;

    ID3D12Device* _Device;

    std::vector<ID3D12CommandAllocator*> _AllocatorPool;
    std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>> _ReadyAllocators;
    std::mutex _AllocatorMutex;
  };

}
