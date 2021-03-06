#pragma once

#include <d3d12.h> //TODO abstract away D3D12_CLEAR_FLAGS, D3D12_VIEWPORT, D3D12_RECT, D3D_PRIMITIVE_TOPOLOGY, D3D12_COMMAND_LIST_TYPE

#include <vector>

namespace Kame {

  class GpuResource;
  class Texture;
  class RenderTarget;
  class RenderProgram;
  class RenderProgramSignature;
  class VertexBuffer;
  class IndexBuffer;

  class CommandList {

  public:

    virtual D3D12_COMMAND_LIST_TYPE GetType() const = 0;

    virtual void ClearTexture(const Texture* texture, const float clearColor[4]) = 0;
    virtual void ClearDepthStencilTexture(const Texture* texture, D3D12_CLEAR_FLAGS clearFlags, float depth = 1.0f, uint8_t stencil = 0) = 0;

    virtual void SetRenderTarget(const RenderTarget& renderTarget) = 0;
    virtual void SetViewport(const D3D12_VIEWPORT& viewport) = 0;
    virtual void SetScissorRect(const D3D12_RECT& scissorRect) = 0;

    virtual void SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitiveTopology) = 0;

    virtual void SetRenderProgram(const RenderProgram* renderProgram) = 0;
    virtual void SetGraphicsRootSignature(const RenderProgramSignature* rootSignature) = 0;

    virtual void SetGraphics32BitConstants(uint32_t rootParameterIndex, uint32_t numConstants, const void* constants) = 0;
    template<typename T>
    void SetGraphics32BitConstants(uint32_t rootParameterIndex, const T& constants) {
      static_assert(sizeof(T) % sizeof(uint32_t) == 0, "Size of type must be a multiple of 4 bytes");
      SetGraphics32BitConstants(rootParameterIndex, sizeof(T) / sizeof(uint32_t), &constants);
    }

    virtual void SetGraphicsDynamicStructuredBuffer(uint32_t slot, size_t numElements, size_t elementSize, const void* bufferData) = 0;
    template<typename T>
    void SetGraphicsDynamicStructuredBuffer(uint32_t slot, const std::vector<T>& bufferData) {
      SetGraphicsDynamicStructuredBuffer(slot, bufferData.size(), sizeof(T), bufferData.data());
    }

    virtual void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData) = 0;
    template<typename T>
    void SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, const T& data) {
      SetGraphicsDynamicConstantBuffer(rootParameterIndex, sizeof(T), &data);
    }

    virtual void SetShaderResourceView(
      uint32_t rootParameterIndex,
      uint32_t descriptorOffset,
      const GpuResource* resource,
      D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
      D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
      UINT firstSubresource = 0,
      UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
      const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr
    ) = 0;

    virtual void SetShaderResourceViewTexture(
      uint32_t rootParameterIndex,
      uint32_t descriptorOffset,
      const Texture* texture,
      D3D12_RESOURCE_STATES stateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
      D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
      UINT firstSubresource = 0,
      UINT numSubresources = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
      const D3D12_SHADER_RESOURCE_VIEW_DESC* srv = nullptr
    ) = 0;

    virtual void SetVertexBuffer(uint32_t slot, const VertexBuffer* vertexBuffer) = 0;
    virtual void SetIndexBuffer(const IndexBuffer* indexBuffer) = 0;

    virtual void SetDynamicVertexBuffer(uint32_t slot, size_t numVertices, size_t vertexSize, const void* vertexBufferData) = 0;
    template<typename T>
    void SetDynamicVertexBuffer(uint32_t slot, const std::vector<T>& vertexBufferData) {
      SetDynamicVertexBuffer(slot, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
    }

    virtual void CopyVertexBuffer(
      VertexBuffer* vertexBuffer, size_t numVertices, size_t vertexStride, const void* vertexBufferData
    ) = 0;
    template<typename T>
    void CopyVertexBuffer(VertexBuffer* vertexBuffer, const std::vector<T>& vertexBufferData) {
      CopyVertexBuffer(vertexBuffer, vertexBufferData.size(), sizeof(T), vertexBufferData.data());
    }

    virtual void CopyIndexBuffer(IndexBuffer* indexBuffer, size_t numIndicies, DXGI_FORMAT indexFormat, const void* indexBufferData) = 0;
    template<typename T>
    void CopyIndexBuffer(IndexBuffer* indexBuffer, const std::vector<T>& indexBufferData) {
      assert(sizeof(T) == 2 || sizeof(T) == 4);
      DXGI_FORMAT indexFormat = (sizeof(T) == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
      CopyIndexBuffer(indexBuffer, indexBufferData.size(), indexFormat, indexBufferData.data());
    }

    virtual void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t startVertex = 0, uint32_t startInstance = 0) = 0;
    virtual void DrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t startIndex = 0, int32_t baseVertex = 0, uint32_t startInstance = 0) = 0;

    virtual void Reset() = 0;
  };

}