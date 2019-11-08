#pragma once

#include "PipelineState.h"

namespace Kame {

  struct PipelineStateStream {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER RasterizerState;
  };

  class GraphicsPipelineState : public PipelineState {

  public: // Functions

    GraphicsPipelineState();

    void SetBlendState(const D3D12_BLEND_DESC& blendDesc);
    void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& rasterizerDesc);
    void SetRasterizerState(D3D12_RASTERIZER_DESC rasterizerDesc);
    void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs);
    void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType);
    void SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, UINT MsaaCount = 1, UINT MsaaQualitiy = 0);
    void SetRenderTargetFormat(D3D12_RT_FORMAT_ARRAY rtvFormats, DXGI_FORMAT dsvFormat);


    void SetRenderTargetFormats(UINT numRtvs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, UINT MsaaCount = 1, UINT MsaaQualitiy = 0);

    void SetVertexShader(const void* byteCode, size_t size);
    void SetPixelShader(const void* byteCode, size_t size);

    void SetVertexShader(const D3D12_SHADER_BYTECODE& binary) { _PipelineStateStream.VS = binary; }
    void SetPixelShader(const D3D12_SHADER_BYTECODE& binary) { _PipelineStateStream.PS = binary; }

    void Finalize();

    //TODO entfernen:
    static void DestroyAll();

  private: // Fields

    PipelineStateStream _PipelineStateStream;
    //D3D12_GRAPHICS_PIPELINE_STATE_DESC _PsoDesc;
    std::shared_ptr<const D3D12_INPUT_ELEMENT_DESC> _InputLayouts;

  };

}