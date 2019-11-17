#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <cstdint>
#include <memory>
#include <queue>
#include <functional>

namespace Kame {

  class CommandList;
  class RootSignature;

  class DynamicDescriptorHeap {

  public:
    DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap = 1024);

    virtual ~DynamicDescriptorHeap();

    void StageDescriptors(uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors);

    void CommitStagedDescriptors(CommandList& commandContext, std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc);
    void CommitStagedDescriptorsForDraw(CommandList& commandContext);
    void CommitStagedDescriptorsForDispatch(CommandList& commandContext);

    D3D12_GPU_DESCRIPTOR_HANDLE CopyDescriptor(CommandList& commandContext, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor);

    void ParseRootSignature(const RootSignature& rootSignature);

    void Reset();

  private: // Fields

    struct DescriptorTableCache {
      DescriptorTableCache() {
        NumDescriptors = 0;
        BaseDescriptor = nullptr;
      }

      void Reset() {
        NumDescriptors = 0;
        BaseDescriptor = nullptr;
      }

      uint32_t NumDescriptors;
      D3D12_CPU_DESCRIPTOR_HANDLE* BaseDescriptor;
    };

    static const uint32_t MaxDescriptorTables = 32;

    D3D12_DESCRIPTOR_HEAP_TYPE _DescriptorHeapType;

    uint32_t _NumDescriptorsPerHeap;
    uint32_t _DescriptorHandleIncrementSize;

    std::unique_ptr<D3D12_CPU_DESCRIPTOR_HANDLE[]> _DescriptorHandleCache;

    DescriptorTableCache _DescriptorTableCache[MaxDescriptorTables];

    uint32_t _DescriptorTableBitMask;
    uint32_t _StaleDescriptorTableBitMask;

    using DescriptorHeapPool = std::queue<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>>;

    DescriptorHeapPool _DescriptorHeapPool;
    DescriptorHeapPool _AvailableDescriptorHeaps;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _CurrentDescriptorHeap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE _CurrentCpuDescriptorHandle;
    CD3DX12_GPU_DESCRIPTOR_HANDLE _CurrentGpuDescriptorHandle;

    uint32_t _NumFreeHandles;

  private: // Functions
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RequestDescriptorHeap();
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap();

    uint32_t ComputeStaleDescriptorCount() const;
  };

}
