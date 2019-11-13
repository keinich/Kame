#include "kmpch.h"
#include "CommandContext.h"
#include "CommandManager.h"
#include "ContextManager.h"
#include "DX12Core.h"
#include "GpuResource.h"
#include "UploadBuffer.h"
#include "PipelineState.h"

namespace Kame {

  CommandContext::CommandContext(D3D12_COMMAND_LIST_TYPE type) :
    _Type(type),
    _CurrentAllocator(nullptr),
    _CommandList(nullptr) {

    _NumBarriersToFlush = 0;

    _UploadBuffer = std::make_unique<UploadBuffer>();

    _CurrentPipelineState = nullptr;
    _CurrentGraphicsRootSignature = nullptr;

    ZeroMemory(_CurrentDescriptorHeaps, sizeof(_CurrentDescriptorHeaps));

  }

  void CommandContext::Reset() {
    assert(_CommandList != nullptr && _CurrentAllocator == nullptr);

    _CurrentAllocator = DX12Core::GetCommandManager()->GetQueue(_Type).RequestAllocator();
    _CommandList->Reset(_CurrentAllocator, nullptr);

    _CurrentPipelineState = nullptr;
    _CurrentGraphicsRootSignature = nullptr;

    _NumBarriersToFlush = 0;

    BindDescriptorHeaps();

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

    _UploadBuffer->Reset();

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

  void CommandContext::SetPipelineState(const PipelineState& pipelineStateToSet) {
    ID3D12PipelineState* pipelineState = pipelineStateToSet.GetPipelineState();
    if (pipelineState == _CurrentPipelineState)
      return;

    _CommandList->SetPipelineState(pipelineState);
    _CurrentPipelineState = pipelineState;
  }

  void CommandContext::SetRootSignature(const RootSignature& rootSignatureToSet) {

    if (rootSignatureToSet.GetRootSignature().Get() == _CurrentGraphicsRootSignature)
      return;

    _CurrentGraphicsRootSignature = rootSignatureToSet.GetRootSignature().Get();
    _CommandList->SetGraphicsRootSignature(_CurrentGraphicsRootSignature);

    //TODO Parse Root Signature
  }

  void CommandContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap) {
    if (_CurrentDescriptorHeaps[type] != heap) {
      _CurrentDescriptorHeaps[type] = heap;
      BindDescriptorHeaps();
    }
  }

  void CommandContext::SetDescriptorHeaps(UINT numHeaps, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]) {
    bool anyChanged = false;

    for (UINT i = 0; i < numHeaps; ++i) {
      if (_CurrentDescriptorHeaps[i] != heaps[i]) {
        _CurrentDescriptorHeaps[i] = heaps[i];
        anyChanged = true;
      }
    }

    if (anyChanged) {
      BindDescriptorHeaps();
    }
  }

  void CommandContext::BindDescriptorHeaps() {
    UINT numNonNullHeaps = 0;
    ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {};

    for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i) {
      ID3D12DescriptorHeap* heapIter = _CurrentDescriptorHeaps[i];
      if (heapIter != nullptr) {
        heapsToBind[numNonNullHeaps++] = heapIter;
      }
    }

    if (numNonNullHeaps > 0) {
      _CommandList->SetDescriptorHeaps(numNonNullHeaps, heapsToBind);
    }

  }

}