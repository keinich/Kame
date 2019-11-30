#pragma once

#include <d3d12.h> //TODO abstract away D3D12_PIPELINE_STATE_STREAM_DESC

namespace Kame {

  class RootSignature;

  class RenderProgram {


  public:

    virtual ~RenderProgram() {};

    virtual void Create() = 0;

    virtual void SetRootSignature(const RootSignature& BindMappings) = 0;

    virtual void SetPrimitiveTopologyType1(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType) = 0;

    virtual void SetRenderTargetFormats1(
      D3D12_RT_FORMAT_ARRAY rtvFormats,
      DXGI_FORMAT DSVFormat = DXGI_FORMAT(),
      UINT MsaaCount = 1,
      UINT MsaaQuality = 0
    ) = 0;

    virtual void SetVertexShader1(const D3D12_SHADER_BYTECODE& Binary) = 0;
    virtual void SetPixelShader1(const D3D12_SHADER_BYTECODE& Binary) = 0;

    virtual void SetInputLayout1(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs) = 0;

  };

}