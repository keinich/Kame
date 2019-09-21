#include "kmpch.h"
#include "CommandQueue.h"

Kame::CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
  _Type(type),
  _CommandQueue(nullptr),
  _Fence(nullptr),
  _CommandAllocatorPool(type) {}

Kame::CommandQueue::~CommandQueue() {
  Shutdown();
}

void Kame::CommandQueue::Create(ID3D12Device* device) {

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

void Kame::CommandQueue::Shutdown() {

  if (_CommandQueue == nullptr)
    return;

  _CommandAllocatorPool.Shutdown();

  CloseHandle(_FenceEventHandle);

  _Fence->Release();
  _Fence = nullptr;

  _CommandQueue->Release();
  _CommandQueue = nullptr;

}

uint64_t Kame::CommandQueue::Signal() {
  uint64_t fenceValueForSignal = ++_FenceValue;
  ThrowIfFailed(_CommandQueue->Signal(_Fence.Get(), _FenceValue));

  return fenceValueForSignal;
}

void Kame::CommandQueue::WaitForFence(uint64_t fenceValue) {

  if (IsFenceComplete(fenceValue))
    return;

  {
    std::lock_guard<std::mutex> LockGuard(_EventMutex);

    _Fence->SetEventOnCompletion(fenceValue, _FenceEventHandle);
    WaitForSingleObject(_FenceEventHandle, INFINITE);
    _LastCompletedFenceValue = fenceValue;
  }
}

bool Kame::CommandQueue::IsFenceComplete(uint64_t fenceValue) {
  if (fenceValue > _LastCompletedFenceValue)
    _LastCompletedFenceValue = std::max(_LastCompletedFenceValue, _Fence->GetCompletedValue());

  return fenceValue <= _LastCompletedFenceValue;
}

void Kame::CommandQueue::WaitForIdle() {
  WaitForFence(Signal());
}
