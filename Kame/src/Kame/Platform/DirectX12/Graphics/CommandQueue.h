#pragma once

#include "CommandAllocatorPool.h"
#include "Kame/Utility/ThreadSafeQueue.h"

namespace Kame {

  class CommandList;

  class KAME_API CommandQueue {

  public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE type);
    ~CommandQueue();

    void Create();
    void Shutdown();

    ID3D12CommandQueue* GetCommandQueue() { return _CommandQueue.Get(); }

    uint64_t Signal();
    void WaitForFence(uint64_t fenceValue);
    bool IsFenceComplete(uint64_t fenceValue);
    void WaitForIdle();
    void Wait(const CommandQueue& other);

    uint64_t ExecuteCommandList(ID3D12CommandList* list); // Mini
    uint64_t CommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList> >& commandLists); // 3dgep
    uint64_t CommandQueue::ExecuteCommandList1(std::shared_ptr<CommandList> commandList);

    ID3D12CommandAllocator* RequestAllocator();
    void DiscardAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

    std::shared_ptr<CommandList> CommandQueue::GetCommandList(); //3dgep

  private:

    ComPtr<ID3D12CommandQueue> _CommandQueue;
    D3D12_COMMAND_LIST_TYPE _Type;

    CommandAllocatorPool _CommandAllocatorPool;

    ComPtr<ID3D12Fence> _Fence;
    HANDLE _FenceEventHandle;
    uint64_t _LastCompletedFenceValue;
    uint64_t _NextFenceValue;
    std::mutex _EventMutex;
    std::mutex _FenceMutex;

    //3dgep Stuff:
    using CommandListEntry = std::tuple<uint64_t, std::shared_ptr<CommandList> >;

    ThreadSafeQueue<CommandListEntry> _InFlightCommandLists;
    ThreadSafeQueue<std::shared_ptr<CommandList> > _AvailableCommandLists;

    // A thread to process in-flight command lists.
    std::thread _ProcessInFlightCommandListsThread;
    std::atomic_bool _bProcessInFlightCommandLists;
    std::mutex _ProcessInFlightCommandListsThreadMutex;
    std::condition_variable _ProcessInFlightCommandListsThreadCV;
  };

}