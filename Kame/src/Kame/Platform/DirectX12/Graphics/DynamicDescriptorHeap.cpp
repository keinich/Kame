#include "kmpch.h"
#include "DynamicDescriptorHeap.h"

#include "DX12Core.h"

#include "CommandContext.h"

namespace Kame {

  DynamicDescriptorHeap::DynamicDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptorsPerHeap) {
    _DescriptorHeapType = heapType;
    _NumDescriptorsPerHeap = numDescriptorsPerHeap;
    _DescriptorTableBitMask = 0;
    _StaleDescriptorTableBitMask = 0;
    _CurrentCpuDescriptorHandle = D3D12_DEFAULT;
    _CurrentGpuDescriptorHandle = D3D12_DEFAULT;
    _NumFreeHandles = 0;

    _DescriptorHandleIncrementSize = DX12Core::Get()->GetDevice()->GetDescriptorHandleIncrementSize(heapType);

    _DescriptorHandleCache = std::make_unique<D3D12_CPU_DESCRIPTOR_HANDLE[]>(_NumDescriptorsPerHeap);
  }

  DynamicDescriptorHeap::~DynamicDescriptorHeap() {}

  void DynamicDescriptorHeap::StageDescriptors(
    uint32_t rootParameterIndex, uint32_t offset, uint32_t numDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE srcDescriptors
  ) {
    if (numDescriptors > _NumDescriptorsPerHeap || rootParameterIndex >= MaxDescriptorTables) {
      throw std::bad_alloc();
    }

    DescriptorTableCache& descriptorTableCache = _DescriptorTableCache[rootParameterIndex];

    if ((offset + numDescriptors) > descriptorTableCache.NumDescriptors) {
      throw std::length_error("Number of descriptors exceeds the number of descriptors in the descriptor table.");
    }

    D3D12_CPU_DESCRIPTOR_HANDLE* dstDescriptor = (descriptorTableCache.BaseDescriptor + offset);
    for (uint32_t i = 0; i < numDescriptors; ++i) {
      dstDescriptor[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(srcDescriptors, i, _DescriptorHandleIncrementSize);
    }

    _StaleDescriptorTableBitMask |= (1 << rootParameterIndex);
  }

  void DynamicDescriptorHeap::CommitStagedDescriptors(
    CommandContext& commandContext,
    std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc
  ) {
    uint32_t numDescriptorsToCommit = ComputeStaleDescriptorCount();

    if (numDescriptorsToCommit == 0) {
      return;
    }

    auto device = DX12Core::GetDevice();
    auto d3D12GraphicsCommandList = commandContext.GetCommandList();
    assert(d3D12GraphicsCommandList != nullptr);

    if (!_CurrentDescriptorHeap || _NumFreeHandles < numDescriptorsToCommit) {
      _CurrentDescriptorHeap = RequestDescriptorHeap();
      _CurrentCpuDescriptorHandle = _CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
      _CurrentGpuDescriptorHandle = _CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
      _NumFreeHandles = _NumDescriptorsPerHeap;

      commandContext.SetDescriptorHeap(_DescriptorHeapType, _CurrentDescriptorHeap.Get());

      _StaleDescriptorTableBitMask = _DescriptorTableBitMask;
    }

    DWORD rootIndex;
    while (_BitScanForward(&rootIndex, _StaleDescriptorTableBitMask)) {
      UINT numSrcDescriptors = _DescriptorTableCache[rootIndex].NumDescriptors;
      D3D12_CPU_DESCRIPTOR_HANDLE* srcDescriptorHandles = _DescriptorTableCache[rootIndex].BaseDescriptor;

      D3D12_CPU_DESCRIPTOR_HANDLE destDescriptorRangeStarts[] = { _CurrentCpuDescriptorHandle };
      UINT destDescriptorRangeSizes[] = { numSrcDescriptors };

      device->CopyDescriptors(
        1,
        destDescriptorRangeStarts,
        destDescriptorRangeSizes,
        numSrcDescriptors,
        srcDescriptorHandles,
        nullptr,
        _DescriptorHeapType
      );

      setFunc(d3D12GraphicsCommandList, rootIndex, _CurrentGpuDescriptorHandle);

      _CurrentCpuDescriptorHandle.Offset(numSrcDescriptors, _DescriptorHandleIncrementSize);
      _CurrentGpuDescriptorHandle.Offset(numSrcDescriptors, _DescriptorHandleIncrementSize);
      _NumFreeHandles -= numSrcDescriptors;

      _StaleDescriptorTableBitMask ^= (1 << rootIndex);

    }
  }

  void DynamicDescriptorHeap::CommitStagedDescriptorsForDraw(CommandContext& commandContext) {
    CommitStagedDescriptors(commandContext, &ID3D12GraphicsCommandList::SetGraphicsRootDescriptorTable);
  }

  void DynamicDescriptorHeap::CommitStagedDescriptorsForDispatch(CommandContext& commandContext) {
    CommitStagedDescriptors(commandContext, &ID3D12GraphicsCommandList::SetComputeRootDescriptorTable);
  }

  D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::CopyDescriptor(
    CommandContext& commandContext, D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor
  ) {
    if (!_CurrentDescriptorHeap || _NumFreeHandles < 1) {
      _CurrentDescriptorHeap = RequestDescriptorHeap();
      _CurrentCpuDescriptorHandle = _CurrentDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
      _CurrentGpuDescriptorHandle = _CurrentDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
      _NumFreeHandles = _NumDescriptorsPerHeap;

      commandContext.SetDescriptorHeap(_DescriptorHeapType, _CurrentDescriptorHeap.Get());

      _StaleDescriptorTableBitMask = _DescriptorTableBitMask;
    }

    auto device = DX12Core::GetDevice();

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = _CurrentGpuDescriptorHandle;
    device->CopyDescriptorsSimple(1, _CurrentCpuDescriptorHandle, cpuDescriptor, _DescriptorHeapType);

    _CurrentCpuDescriptorHandle.Offset(1, _DescriptorHandleIncrementSize);
    _CurrentGpuDescriptorHandle.Offset(1, _DescriptorHandleIncrementSize);
    _NumFreeHandles -= 1;

    return gpuHandle;
  }

  void DynamicDescriptorHeap::ParseRootSignature(const RootSignature& rootSignature) {
    _StaleDescriptorTableBitMask = 0;

    const auto& rootSignatureDesc = rootSignature.GetRootSignatureDesc();

    _DescriptorTableBitMask = rootSignature.GetDescriptorTableBitMask(_DescriptorHeapType);
    uint32_t descriptorTableBitMask = _DescriptorTableBitMask;

    uint32_t currentOffset = 0;
    DWORD rootIndex;
    while (_BitScanForward(&rootIndex, descriptorTableBitMask) && rootIndex < rootSignatureDesc.NumParameters) {
      uint32_t numDescriptors = rootSignature.GetNumDescriptors(rootIndex);

      DescriptorTableCache& descriptorTableCache = _DescriptorTableCache[rootIndex];
      descriptorTableCache.NumDescriptors = numDescriptors;
      descriptorTableCache.BaseDescriptor = _DescriptorHandleCache.get() + currentOffset;

      currentOffset += numDescriptors;

      descriptorTableBitMask ^= (1 << rootIndex);
    }
    assert(
      currentOffset <= _NumDescriptorsPerHeap &&
      "The root signature requires more than the maximum number of descriptors per descriptor heap. Consider increasing the maximum number of descriptors per descriptor heap."
    );
  }

  void DynamicDescriptorHeap::Reset() {
    _AvailableDescriptorHeaps = _DescriptorHeapPool;
    _CurrentDescriptorHeap.Reset();
    _CurrentCpuDescriptorHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    _CurrentGpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(D3D12_DEFAULT);
    _NumFreeHandles = 0;
    _DescriptorTableBitMask = 0;
    _StaleDescriptorTableBitMask = 0;

    for (int i = 0; i < MaxDescriptorTables; ++i) {
      _DescriptorTableCache[i].Reset();
    }
  }

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::RequestDescriptorHeap() {
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    if (!_AvailableDescriptorHeaps.empty()) {
      descriptorHeap = _AvailableDescriptorHeaps.front();
      _AvailableDescriptorHeaps.pop();
    }
    else {
      descriptorHeap = CreateDescriptorHeap();
      _DescriptorHeapPool.push(descriptorHeap);
    }

    return descriptorHeap;
  }

  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DynamicDescriptorHeap::CreateDescriptorHeap() {
    auto device = DX12Core::GetDevice();

    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
    descriptorHeapDesc.Type = _DescriptorHeapType;
    descriptorHeapDesc.NumDescriptors = _NumDescriptorsPerHeap;
    descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    ThrowIfFailed(device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
  }

  uint32_t DynamicDescriptorHeap::ComputeStaleDescriptorCount() const {
    uint32_t numStaleDescriptors = 0;
    DWORD i;
    DWORD staleDescriptorsBitMask = _StaleDescriptorTableBitMask;

    while (_BitScanForward(&i, staleDescriptorsBitMask)) {
      numStaleDescriptors += _DescriptorTableCache[i].NumDescriptors;
      staleDescriptorsBitMask ^= (1 << i);
    }

    return numStaleDescriptors;
  }

}