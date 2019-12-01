#include "kmpch.h"

#include "Buffer.h"

namespace Kame {

  Buffer::Buffer(const std::wstring& name)
    : GpuResourceDx12(name) {}

  Buffer::Buffer(const D3D12_RESOURCE_DESC& resDesc,
    size_t numElements, size_t elementSize,
    const std::wstring& name)
    : GpuResourceDx12(resDesc, nullptr, name) {}

}