#include "kmpch.h"

#include "CommandAllocatorPool.h"

namespace Kame {

  class KAME_API CommandQueue {

  public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE type);
    ~CommandQueue();

    void Create(ID3D12Device* device);
    void Shutdown();

    ID3D12CommandQueue* GetCommandQueue() { return _CommandQueue.Get(); }

    uint64_t Signal();
    void WaitForFence(uint64_t fenceValue);
    bool IsFenceComplete(uint64_t fenceValue);
    void WaitForIdle();

    ID3D12CommandAllocator* RequestAllocator();
    void DiscardAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator);

  private:

    ComPtr<ID3D12CommandQueue> _CommandQueue;
    D3D12_COMMAND_LIST_TYPE _Type;

    CommandAllocatorPool _CommandAllocatorPool;

    ComPtr<ID3D12Fence> _Fence;
    HANDLE _FenceEventHandle;
    uint64_t _LastCompletedFenceValue;
    uint64_t _FenceValue;
    std::mutex _EventMutex;
  };

}