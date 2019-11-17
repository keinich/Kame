#include "kmpch.h"
#include "CommandQueue.h"
#include "ResourceStateTracker.h"
#include "CommandList.h"
#include "DX12Core.h"

namespace Kame {
  CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
    _Type(type),
    _CommandQueue(nullptr),
    _Fence(nullptr),
    _CommandAllocatorPool(type),
    _LastCompletedFenceValue((uint64_t)type << 56),
    _NextFenceValue((uint64_t)type << 56 | 1) {
    Create();
  }

  CommandQueue::~CommandQueue() {
    Shutdown();
  }

  void CommandQueue::Create() {

    auto device = DX12Core::GetDevice();

    // Create the D3D12 CommandQueue
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = _Type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_CommandQueue)));

    // Create the fence
    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_Fence)));
    _Fence->SetName(L"CommandListManager::_CommandQueue");
    _Fence->Signal((uint64_t)_Type << 56);
    _FenceEventHandle = CreateEvent(nullptr, false, false, nullptr);
    assert(_FenceEventHandle);

    // Create the CommandAllocator-Pool
    _CommandAllocatorPool.Create(device);

  }

  void CommandQueue::Shutdown() {

    if (_CommandQueue == nullptr)
      return;

    _CommandAllocatorPool.Shutdown();

    CloseHandle(_FenceEventHandle);

    _Fence->Release();
    _Fence = nullptr;

    _CommandQueue->Release();
    _CommandQueue = nullptr;

  }

  uint64_t CommandQueue::Signal() {
    //uint64_t fenceValueForSignal = ++_NextFenceValue;
    ThrowIfFailed(_CommandQueue->Signal(_Fence.Get(), _NextFenceValue));

    return _NextFenceValue++;
  }

  void CommandQueue::WaitForFence(uint64_t fenceValue) {

    if (IsFenceComplete(fenceValue))
      return;

    {
      std::lock_guard<std::mutex> LockGuard(_EventMutex);

      _Fence->SetEventOnCompletion(fenceValue, _FenceEventHandle);
      WaitForSingleObject(_FenceEventHandle, INFINITE);
      _LastCompletedFenceValue = fenceValue;
    }
  }

  bool CommandQueue::IsFenceComplete(uint64_t fenceValue) {
    if (fenceValue > _LastCompletedFenceValue)
      _LastCompletedFenceValue = std::max(_LastCompletedFenceValue, _Fence->GetCompletedValue());

    return fenceValue <= _LastCompletedFenceValue;
  }

  void CommandQueue::WaitForIdle() {
    WaitForFence(Signal());
  }

  void CommandQueue::Wait(const CommandQueue& other) {
    _CommandQueue->Wait(other._Fence.Get(), other._NextFenceValue);
  }

  uint64_t CommandQueue::ExecuteCommandList(ID3D12CommandList* list) {

    std::lock_guard<std::mutex> LockGuard(_FenceMutex);

    ThrowIfFailed(((ID3D12GraphicsCommandList*)list)->Close());

    _CommandQueue->ExecuteCommandLists(1, &list);

    _CommandQueue->Signal(_Fence.Get(), _NextFenceValue);

    return _NextFenceValue++;

  }

  uint64_t CommandQueue::ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList> >& commandLists) {
    ResourceStateTracker::Lock();

    // Command lists that need to put back on the command list queue.
    std::vector<std::shared_ptr<CommandList> > toBeQueued;
    toBeQueued.reserve(commandLists.size() * 2);        // 2x since each command list will have a pending command list.

    // Generate mips command lists.
    std::vector<std::shared_ptr<CommandList> > generateMipsCommandLists;
    generateMipsCommandLists.reserve(commandLists.size());

    // Command lists that need to be executed.
    std::vector<ID3D12CommandList*> d3d12CommandLists;
    d3d12CommandLists.reserve(commandLists.size() * 2); // 2x since each command list will have a pending command list.

    for (auto commandList : commandLists) {
      auto pendingCommandList = GetCommandList();
      bool hasPendingBarriers = commandList->Close(*pendingCommandList);
      pendingCommandList->Close();
      // If there are no pending barriers on the pending command list, there is no reason to 
      // execute an empty command list on the command queue.
      if (hasPendingBarriers) {
        d3d12CommandLists.push_back(pendingCommandList->GetGraphicsCommandList().Get());
      }
      d3d12CommandLists.push_back(commandList->GetGraphicsCommandList().Get());

      toBeQueued.push_back(pendingCommandList);
      toBeQueued.push_back(commandList);

      auto generateMipsCommandList = commandList->GetGenerateMipsCommandList();
      if (generateMipsCommandList) {
        generateMipsCommandLists.push_back(generateMipsCommandList);
      }
    }

    UINT numCommandLists = static_cast<UINT>(d3d12CommandLists.size());
    _CommandQueue->ExecuteCommandLists(numCommandLists, d3d12CommandLists.data());
    uint64_t fenceValue = Signal();

    ResourceStateTracker::Unlock();

    // Queue command lists for reuse.
    for (auto commandList : toBeQueued) {
      _InFlightCommandLists.Push({ fenceValue, commandList });
    }

    // If there are any command lists that generate mips then execute those
    // after the initial resource command lists have finished.
    if (generateMipsCommandLists.size() > 0) {
      auto computeQueue = DX12Core::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE);
      computeQueue->Wait(*this);
      computeQueue->ExecuteCommandLists(generateMipsCommandLists);
    }

    return fenceValue;
  }

  uint64_t CommandQueue::ExecuteCommandList1(std::shared_ptr<CommandList> commandList) {
    return ExecuteCommandLists(std::vector<std::shared_ptr<CommandList> >({ commandList }));
  }

  std::shared_ptr<CommandList> CommandQueue::GetCommandList() {
    std::shared_ptr<CommandList> commandList;

    // If there is a command list on the queue.
    if (!_AvailableCommandLists.Empty()) {
      _AvailableCommandLists.TryPop(commandList);
    }
    else {
      // Otherwise create a new command list.
      // TODO allocate ID3D12CommandAllocator !!
      commandList = std::make_shared<CommandList>(_Type);
    }

    return commandList;
  }

  ID3D12CommandAllocator* Kame::CommandQueue::RequestAllocator() {
    uint64_t completedFenceValue = _Fence->GetCompletedValue();
    return _CommandAllocatorPool.RequestAllocator(completedFenceValue);
  }

  void CommandQueue::DiscardAllocator(uint64_t fenceValue, ID3D12CommandAllocator* allocator) {
    _CommandAllocatorPool.DiscardAllocator(fenceValue, allocator);
  }

}
