#pragma once

#include <d3d12.h> //TODO abstract away D3D12_PIPELINE_STATE_STREAM_DESC, CD3DX12_RASTERIZER_DESC, D3D_ROOT_SIGNATURE_VERSION

namespace Kame {

  class RenderProgramSignature;

  class RenderProgram {


  public:

    virtual ~RenderProgram() {};

    virtual void Create() = 0;

    virtual size_t GetIdentifier() = 0;

    const virtual RenderProgramSignature* GetSignature() const = 0;
    virtual void SetRootSignature(const RenderProgramSignature* signature) = 0;

    virtual void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType) = 0;

    virtual void SetRenderTargetFormats(
      D3D12_RT_FORMAT_ARRAY rtvFormats,
      DXGI_FORMAT DSVFormat = DXGI_FORMAT(),
      UINT MsaaCount = 1,
      UINT MsaaQuality = 0
    ) = 0;

    virtual void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) = 0;
    virtual void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary) = 0;

    virtual void SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs) = 0;

    virtual void SetRasterizer(const CD3DX12_RASTERIZER_DESC& rasterizerDesc) = 0;
    virtual void SetBlendDescription(const CD3DX12_BLEND_DESC& blendDesc) = 0;

  };

}