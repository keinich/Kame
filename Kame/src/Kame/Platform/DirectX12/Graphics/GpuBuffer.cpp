#include "kmpch.h"
#include "GpuBuffer.h"

namespace Kame {
  
  GpuBuffer::GpuBuffer(const std::wstring& name)
    : GpuResource(name) {}

  GpuBuffer::GpuBuffer(
    const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name)
    : GpuResource(resDesc, nullptr, name) {
    CreateViews(numElements, elementSize);
  }

  void GpuBuffer::CreateViews(size_t numElements, size_t elementSize) {
    throw std::exception("Unimplemented function.");
  }

}