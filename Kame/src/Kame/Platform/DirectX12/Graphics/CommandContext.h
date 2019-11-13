#pragma once

#include "kmpch.h"

namespace Kame {

  class GraphicsContext;
  class GpuResource;
  class UploadBuffer;
  class PipelineState;
  class RootSignature;

  class CommandContext {

    friend class ContextManager;

  private:

    CommandContext(D3D12_COMMAND_LIST_TYPE type);

    void Reset();

  public:

    ~CommandContext();

    void Initialize();

    static CommandContext& Begin(const std::wstring ID = L"");
    uint64_t Finish(bool waitForCompletion = false);

    GraphicsContext& GetGraphicsContext();

    void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);
    void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState, bool flushImmediate = false);

    inline void FlushResourceBarriers();

    void SetPipelineState(const PipelineState& pipelineState);
    void SetRootSignature(const RootSignature& rootSignatureToSet);

    void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* heap);
    void SetDescriptorHeaps(UINT numHeaps, D3D12_DESCRIPTOR_HEAP_TYPE types[], ID3D12DescriptorHeap* heaps[]);

    // TODO nur fürs Rantasten gedacht
    inline ID3D12GraphicsCommandList* GetCommandList() { return _CommandList; };
    inline ID3D12CommandAllocator* GetCurrentAllocator() { return _CurrentAllocator; };

  protected: // Fields

    ID3D12GraphicsCommandList* _CommandList;
    ID3D12CommandAllocator* _CurrentAllocator;

    D3D12_COMMAND_LIST_TYPE _Type;

    UINT _NumBarriersToFlush;
    D3D12_RESOURCE_BARRIER _ResourceBarrierBuffer[16];

    ID3D12PipelineState* _CurrentPipelineState;
    ID3D12RootSignature* _CurrentGraphicsRootSignature;

    std::unique_ptr<UploadBuffer> _UploadBuffer;

    ID3D12DescriptorHeap* _CurrentDescriptorHeaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

  protected: // Methods

    void BindDescriptorHeaps();
  };

}
