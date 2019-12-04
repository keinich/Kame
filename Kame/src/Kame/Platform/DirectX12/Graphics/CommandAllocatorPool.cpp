#include "kmpch.h"
#include "kmpch.h"
#include "CommandAllocatorPool.h"
#include <Kame/Core/DebugUtilities.h>

namespace Kame {

  CommandAllocatorPool::CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type) :
    _CommandListType(type),
    _Device(nullptr) {}

  CommandAllocatorPool::~CommandAllocatorPool() {
    Shutdown();
  }

  void CommandAllocatorPool::Create(ID3D12Device* device) {
    _Device = device;
  }

  void CommandAllocatorPool::Shutdown() {
    for (size_t i = 0; i < _AllocatorPool.size(); ++i)
      _AllocatorPool[i]->Release();

    _AllocatorPool.clear();
  }

  ID3D12CommandAllocator* CommandAllocatorPool::RequestAllocator(uint64_t completedFenceValue) {
    
    std::lock_guard<std::mutex> LockGuard(_AllocatorMutex);

    ID3D12CommandAllocator* allocator = nullptr;

    if (!_ReadyAllocators.empty()) {
      std::pair<uint64_t, ID3D12CommandAllocator*>& allocatorPair = _ReadyAllocators.front();

      if (allocatorPair.first <= completedFenceValue) {
        allocator = allocatorPair.second;
        ThrowIfFailed(allocator->Reset());
        _ReadyAllocators.pop();
      }
    }

    if (allocator == nullptr) {
      ThrowIfFailed(_Device->CreateCommandAllocator(_CommandListType, IID_PPV_ARGS(&allocator)));
      _AllocatorPool.push_back(allocator);
    }

    return allocator;
  }

  void CommandAllocatorPool::DiscardAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator) {

    std::lock_guard<std::mutex> LockGuard(_AllocatorMutex);

    _ReadyAllocators.push(std::make_pair(fenceValue, allocator));
  }

}

