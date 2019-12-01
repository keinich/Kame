#pragma once

#include <d3d12.h> //TODO abstract away D3D12_CLEAR_FLAGS, D3D12_VIEWPORT, D3D12_RECT, D3D_PRIMITIVE_TOPOLOGY

namespace Kame {

  class GpuResource;
  class Texture;
  class RenderTarget;
  class RenderProgram;
  class RenderProgramSignature;

  class CommandList {

  public:

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

  };

}