#include "kmpch.h"
#include "DescriptorAllocatorPage.h"

#include "DX12Core.h"

Kame::DescriptorAllocatorPage::DescriptorAllocatorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
  _HeapType = type;
  _NumDescriptorsInHeap = numDescriptors;

  auto device = DX12Core::GetDevice();

  D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
  heapDesc.Type = _HeapType;
  heapDesc.NumDescriptors = _NumDescriptorsInHeap;

  ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&_D3D12DescriptorHeap)));

  _BaseDescriptor = _D3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
  _DescriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(_HeapType);
  _NumFreeHandles = _NumDescriptorsInHeap;

  AddNewBlock(0, _NumFreeHandles);
}

D3D12_DESCRIPTOR_HEAP_TYPE Kame::DescriptorAllocatorPage::GetHeapType() const {
  return _HeapType;
}

bool Kame::DescriptorAllocatorPage::HasSpace(uint32_t numDescriptors) const {
  return _FreeListBySize.lower_bound(numDescriptors) != _FreeListBySize.end();
}

uint32_t Kame::DescriptorAllocatorPage::GetNumFreeHandles() const {
  return _NumFreeHandles;
}

Kame::DescriptorAllocation Kame::DescriptorAllocatorPage::Allocate(uint32_t numDescriptors) {

  std::lock_guard<std::mutex> lock(_AllocationMutex);

  if (numDescriptors > _NumFreeHandles) {
    return DescriptorAllocation();
  }

  auto smallestBlockIt = _FreeListBySize.lower_bound(numDescriptors);
  if (smallestBlockIt == _FreeListBySize.end()) {
    return DescriptorAllocation();
  }


  auto blockSize = smallestBlockIt->first;
  auto offsetIt = smallestBlockIt->second;
  auto offset = offsetIt->first;

  _FreeListBySize.erase(smallestBlockIt);
  _FreeListByOffset.erase(offsetIt);

  auto newOffset = offset + numDescriptors;
  auto newSize = blockSize - numDescriptors;

  if (newSize > 0) {
    AddNewBlock(newOffset, newSize);
  }

  _NumFreeHandles -= numDescriptors;

  return DescriptorAllocation(
    CD3DX12_CPU_DESCRIPTOR_HANDLE(_BaseDescriptor, offset, _DescriptorHandleIncrementSize),
    numDescriptors, _DescriptorHandleIncrementSize, shared_from_this()
  );
}

void Kame::DescriptorAllocatorPage::Free(DescriptorAllocation&& descriptor, uint64_t frameNumber) {
  auto offset = ComputeOffset(descriptor.GetDescriptorHandle());

  std::lock_guard < std::mutex> lock(_AllocationMutex);

  _StaleDescriptors.emplace(offset, descriptor.GetNumHandles(), frameNumber);

}

void Kame::DescriptorAllocatorPage::ReleaseStaleDescriptors(uint64_t frameNumber) {

  std::lock_guard<std::mutex> lock(_AllocationMutex);

  while (!_StaleDescriptors.empty() && _StaleDescriptors.front().FrameNumber <= frameNumber) {
    auto& staleDescriptor = _StaleDescriptors.front();

    auto offset = staleDescriptor.Offset;
    auto numDescriptors = staleDescriptor.Size;

    FreeBlock(offset, numDescriptors);
    _StaleDescriptors.pop();
  }

}

uint32_t Kame::DescriptorAllocatorPage::ComputeOffset(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
  return static_cast<uint32_t>(handle.ptr - _BaseDescriptor.ptr) / _DescriptorHandleIncrementSize;
}

void Kame::DescriptorAllocatorPage::AddNewBlock(uint32_t offset, uint32_t numDescriptors) {
  auto offsetIt = _FreeListByOffset.emplace(offset, numDescriptors);
  auto sizeIt = _FreeListBySize.emplace(numDescriptors, offsetIt.first);
  offsetIt.first->second.FreeListBySizeIterator = sizeIt;
}

void Kame::DescriptorAllocatorPage::FreeBlock(uint32_t offset, uint32_t numDescriptors) {

  auto nextBlockIt = _FreeListByOffset.upper_bound(offset);
  auto prevBlockIt = nextBlockIt;

  if (prevBlockIt != _FreeListByOffset.begin()) {
    --prevBlockIt;
  }
  else {
    prevBlockIt = _FreeListByOffset.end();
  }

  _NumFreeHandles += numDescriptors;

  if (
    prevBlockIt != _FreeListByOffset.end() &&
    offset == prevBlockIt->first + prevBlockIt->second.Size
    ) {
    // The previous block is exactly in front of the block that is to be freed.
    //
    // PrevBlock.Offset           Offset
    // |                          |
    // |<-----PrevBlock.Size----->|<------Size-------->|    

    offset = prevBlockIt->first;
    numDescriptors += prevBlockIt->second.Size;

    _FreeListBySize.erase(prevBlockIt->second.FreeListBySizeIterator);
    _FreeListByOffset.erase(prevBlockIt);
  }

  if (
    nextBlockIt != _FreeListByOffset.end() &&
    offset + numDescriptors == nextBlockIt->first
    ) {
    // The next block is exactly behind the block that is to be freed.
    //
    // Offset               NextBlock.Offset 
    // |                    |
    // |<------Size-------->|<-----NextBlock.Size----->|

    numDescriptors += nextBlockIt->second.Size;

    _FreeListBySize.erase(nextBlockIt->second.FreeListBySizeIterator);
    _FreeListByOffset.erase(nextBlockIt);
  }

  AddNewBlock(offset, numDescriptors);
}
