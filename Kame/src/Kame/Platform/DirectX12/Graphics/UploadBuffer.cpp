#include "kmpch.h"
#include "UploadBuffer.h"
#include "DX12Core.h"
#include <new>
#include "Kame/Math/MathCommon.h"

namespace Kame {

  UploadBuffer::UploadBuffer(size_t pageSize) {
    _PageSize = pageSize;
  }

  UploadBuffer::Allocation UploadBuffer::Allocate(size_t sizeInBytes, size_t alignment) {

    if (sizeInBytes > _PageSize) {
      throw std::bad_alloc();
    }

    if (!_CurrentPage || !_CurrentPage->HasSpace(sizeInBytes, alignment)) {
      _CurrentPage = RequestPage();
    }

    return _CurrentPage->Allocate(sizeInBytes, alignment);

  }

  void UploadBuffer::Reset() {

    _CurrentPage = nullptr;
    _AvailablePages = _PagePool;

    for (auto page : _AvailablePages) {
      page->Reset();
    }

  }

  std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage() {

    std::shared_ptr<Page> page;

    if (_AvailablePages.empty()) {
      page = _AvailablePages.front();
    }
    else {
      page = std::make_shared<Page>(_PageSize);
      _PagePool.push_back(page);
    }

    return page;
  }

  UploadBuffer::Page::Page(size_t sizeInBytes)
    : _PageSize(sizeInBytes)
    , _Offset(0)
    , _CpuPtr(nullptr)
    , _GpuPtr(D3D12_GPU_VIRTUAL_ADDRESS(0)) {

    auto device = DX12Core::GetDevice();

    ThrowIfFailed(device->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
      D3D12_HEAP_FLAG_NONE,
      &CD3DX12_RESOURCE_DESC::Buffer(_PageSize),
      D3D12_RESOURCE_STATE_GENERIC_READ,
      nullptr,
      IID_PPV_ARGS(&_D3D12Resource)
    ));

    _GpuPtr = _D3D12Resource->GetGPUVirtualAddress();
    _D3D12Resource->Map(0, nullptr, &_CpuPtr);
  }

  UploadBuffer::Page::~Page() {
    _D3D12Resource->Unmap(0, nullptr);
    _CpuPtr = nullptr;
    _GpuPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
  }

  bool UploadBuffer::Page::HasSpace(size_t sizeInBytes, size_t alignment) const {
    size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
    size_t alignedOffset = Math::AlignUp(_Offset, alignment);

    return alignedOffset + alignedSize <= _PageSize;
  }

  UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInBytes, size_t alignment) {
    
    if (!HasSpace(sizeInBytes, alignment)) {
      throw std::bad_alloc();
    }

    size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
    _Offset = Math::AlignUp(_Offset, alignment);

    Allocation allocation;
    allocation.Cpu = static_cast<uint8_t*>(_CpuPtr) + _Offset;
    allocation.Gpu = _GpuPtr + _Offset;

    _Offset += alignedSize;

    return allocation;
  }

  void UploadBuffer::Page::Reset() {
    _Offset = 0;
  }

}
