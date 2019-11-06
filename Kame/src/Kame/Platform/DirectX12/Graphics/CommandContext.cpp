#include "kmpch.h"
#include "CommandContext.h"
#include "CommandManager.h"
#include "ContextManager.h"
#include "DX12Core.h"
#include "GpuResource.h"

namespace Kame {

  CommandContext::CommandContext(D3D12_COMMAND_LIST_TYPE type) :
    _Type(type),
    _CurrentAllocator(nullptr),
    _CommandList(nullptr) {

    _NumBarriersToFlush = 0;
  }

  void CommandContext::Reset() {
    assert(_CommandList != nullptr && _CurrentAllocator == nullptr);

    _CurrentAllocator = DX12Core::GetCommandManager()->GetQueue(_Type).RequestAllocator();
    _CommandList->Reset(_CurrentAllocator, nullptr);
  }

  CommandContext::~CommandContext() {
    if (_CommandList != nullptr)
      _CommandList->Release();
  }

  void CommandContext::Initialize() {
    DX12Core::GetCommandManager()->CreateNewCommandList(_Type, &_CommandList, &_CurrentAllocator);
  }

  CommandContext& CommandContext::Begin(const std::wstring ID) {
    CommandContext* newContext = DX12Core::GetContextManager()->AllocateContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
    return *newContext;
  }

  uint64_t CommandContext::Finish(bool waitForCompletion) {

    FlushResourceBarriers();

    CommandQueue& queue = DX12Core::GetCommandManager()->GetQueue(_Type);
    uint64_t fenceValue = queue.ExecuteCommandList(_CommandList);
    queue.DiscardAllocator(fenceValue, _CurrentAllocator);
    _CurrentAllocator = nullptr;

    if (waitForCompletion)
      DX12Core::GetCommandManager()->WaitForFence(fenceValue);

    DX12Core::GetContextManager()->FreeContext(this);

    return fenceValue;
  }

  GraphicsContext& CommandContext::GetGraphicsContext() {
    assert(_Type != D3D12_COMMAND_LIST_TYPE_COMPUTE, L"Cannot convert async compute context to graphics");
    return reinterpret_cast<GraphicsContext&>(*this);
  }

  void CommandContext::TransitionResource(
    ID3D12Resource* resource,
    D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState,
    bool flushImmediate
  ) {

    if (oldState != newState) {
      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource,
        oldState, newState
      );
      _ResourceBarrierBuffer[_NumBarriersToFlush++] = barrier;
    }
    if (flushImmediate || _NumBarriersToFlush == 16)
      FlushResourceBarriers();

  }

  void CommandContext::TransitionResource(
    GpuResource& resource,
    D3D12_RESOURCE_STATES newState,
    bool flushImmediate
  ) {

    D3D12_RESOURCE_STATES oldState = resource._UsageState;

    if (oldState != newState) {
      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource._Resource1.Get(),
        oldState, newState
      );
      _ResourceBarrierBuffer[_NumBarriersToFlush++] = barrier;

      resource._UsageState = newState;
    }

    if (flushImmediate || _NumBarriersToFlush == 16)
      FlushResourceBarriers();
  }

  inline void CommandContext::FlushResourceBarriers() {
    if (_NumBarriersToFlush > 0) {
      _CommandList->ResourceBarrier(_NumBarriersToFlush, _ResourceBarrierBuffer);
      _NumBarriersToFlush = 0;
    }
  }

}