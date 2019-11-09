#include "kmpch.h"
#include "DescriptorAllocation.h"
#include "Kame/Application.h"
#include "DescriptorAllocatorPage.h"

namespace Kame {

  DescriptorAllocation::DescriptorAllocation() :
    _Descriptor{ 0 }
    , _NumHandles(0)
    , _DescriptorSize(0)
    , _Page(nullptr) {}

  DescriptorAllocation::DescriptorAllocation(
    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle,
    uint32_t numHandles,
    uint32_t descriptorSize,
    std::shared_ptr<DescriptorAllocatorPage> page
  ) :
    _Descriptor(descriptorHandle)
    , _NumHandles(numHandles)
    , _DescriptorSize(descriptorSize)
    , _Page(page) {}

  DescriptorAllocation::~DescriptorAllocation() {
    Free();
  }

  DescriptorAllocation::DescriptorAllocation(DescriptorAllocation&& allocation) noexcept :
    _Descriptor(allocation._Descriptor)
    , _NumHandles(allocation._NumHandles)
    , _DescriptorSize(allocation._DescriptorSize)
    , _Page(allocation._Page) {

    allocation._Descriptor.ptr = 0;
    allocation._NumHandles = 0;
    allocation._DescriptorSize = 0;
  }

  DescriptorAllocation& DescriptorAllocation::operator=(DescriptorAllocation&& other) noexcept {
    Free();

    _Descriptor = other._Descriptor;
    _NumHandles = other._NumHandles;
    _DescriptorSize = other._DescriptorSize;
    _Page = other._Page;

    other._Descriptor.ptr = 0;
    other._NumHandles = 0;
    other._DescriptorSize = 0;

    return *this;
  }

  bool DescriptorAllocation::IsNull() const {
    return _Descriptor.ptr == 0;
  }

  D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::GetDescriptorHandle(uint32_t offset) const {
    assert(offset < _NumHandles);
    return { _Descriptor.ptr + (_DescriptorSize * offset) };
  }

  uint32_t DescriptorAllocation::GetNumHandles() const {
    return _NumHandles;
  }

  std::shared_ptr<DescriptorAllocatorPage> DescriptorAllocation::GetDescriptorAllocatorPage() const {
    return _Page;
  }

  void DescriptorAllocation::Free() {
    if (!IsNull() && _Page) {
      _Page->Free(std::move(*this), Application::Get().GetFrameCount());

      _Descriptor.ptr = 0;
      _NumHandles = 0;
      _DescriptorSize = 0;
      _Page.reset();
    }
  }




}