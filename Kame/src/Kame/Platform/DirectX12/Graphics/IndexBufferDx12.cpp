#include "kmpch.h"

#include "IndexBufferDx12.h"

#include "cassert"

namespace Kame {

  IndexBufferDx12::IndexBufferDx12(const std::wstring& name)
    : Buffer(name)
    , m_NumIndicies(0)
    , m_IndexFormat(DXGI_FORMAT_UNKNOWN)
    , m_IndexBufferView({}) {}

  IndexBufferDx12::~IndexBufferDx12() {}

  void IndexBufferDx12::CreateViews(size_t numElements, size_t elementSize) {
    assert(elementSize == 2 || elementSize == 4 && "Indices must be 16, or 32-bit integers.");

    m_NumIndicies = numElements;
    m_IndexFormat = (elementSize == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    m_IndexBufferView.BufferLocation = m_d3d12Resource->GetGPUVirtualAddress();
    m_IndexBufferView.SizeInBytes = static_cast<UINT>(numElements * elementSize);
    m_IndexBufferView.Format = m_IndexFormat;
  }

  D3D12_CPU_DESCRIPTOR_HANDLE IndexBufferDx12::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    throw std::exception("IndexBuffer::GetShaderResourceView should not be called.");
  }

  D3D12_CPU_DESCRIPTOR_HANDLE IndexBufferDx12::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    throw std::exception("IndexBuffer::GetUnorderedAccessView should not be called.");
  }

}