#include "kmpch.h"
#include "VertexBuffer.h"

namespace Kame {

  VertexBuffer::VertexBuffer(const std::wstring& name)
    : GpuBuffer(name)
    , _NumVertices(0)
    , _VertexStride(0)
    , _VertexBufferView({}) {}

  VertexBuffer::~VertexBuffer() {}

  void VertexBuffer::CreateViews(size_t numElements, size_t elementSize) {
    _NumVertices = numElements;
    _VertexStride = elementSize;

    _VertexBufferView.BufferLocation = _Resource1->GetGPUVirtualAddress();
    _VertexBufferView.SizeInBytes = static_cast<UINT>(_NumVertices * _VertexStride);
    _VertexBufferView.StrideInBytes = static_cast<UINT>(_VertexStride);
  }

  D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    throw std::exception("VertexBuffer::GetShaderResourceView should not be called.");
  }

  D3D12_CPU_DESCRIPTOR_HANDLE VertexBuffer::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    throw std::exception("VertexBuffer::GetUnorderedAccessView should not be called.");
  }
}