#pragma once

#include <d3d12.h>

#include "RootSignatureDx12.h"
#include <Kame/Graphics/RenderApi/RenderProgram.h>

namespace Kame {

  struct PipelineStateStream {
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
  };

  struct PipelineStateStreamWithoutRtvs {
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
    CD3DX12_PIPELINE_STATE_STREAM_VS VS;
    CD3DX12_PIPELINE_STATE_STREAM_PS PS;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;

    PipelineStateStreamWithoutRtvs() {}

    void Init(PipelineStateStream pss) {
      pRootSignature = pss.pRootSignature;
      InputLayout = pss.InputLayout;
      PrimitiveTopologyType = pss.PrimitiveTopologyType;
      VS = pss.VS;
      PS = pss.PS;
      Rasterizer = pss.Rasterizer;
      DSVFormat = pss.DSVFormat;
      BlendDesc = pss.BlendDesc;
    }
  };

  class RenderProgramDx12 : public RenderProgram {
  public:

    RenderProgramDx12() :
      m_RootSignature(nullptr),
      _NumInputElements(0),
      _InputElementDescs(nullptr),
      _Identifier(0) {}

    virtual ~RenderProgramDx12() override {}

    static void DestroyAll(void);

    virtual size_t GetIdentifier() override { return _Identifier; }


    const RootSignatureDx12& GetRootSignature(void) const {
      assert(m_RootSignature != nullptr);
      return *m_RootSignature;
    }

    const virtual RenderProgramSignature* GetSignature() const override { return m_RootSignature; }

    ID3D12PipelineState* GetPipelineStateObject(void) const { return m_PSO; }

    virtual void Create() override;

    virtual void SetRootSignature(const RenderProgramSignature* signature) override;

    virtual void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType) override {
      _PipelineStateStream.PrimitiveTopologyType = TopologyType;
    }
    virtual void SetRenderTargetFormats(
      D3D12_RT_FORMAT_ARRAY rtvFormats,
      DXGI_FORMAT DSVFormat = DXGI_FORMAT(),
      UINT MsaaCount = 1,
      UINT MsaaQuality = 0
    ) override {
      _RtvFormats = rtvFormats;
      _PipelineStateStream.DSVFormat = DSVFormat;
    }

    virtual void SetVertexShader(const D3D12_SHADER_BYTECODE& Binary) override { _PipelineStateStream.VS = Binary; }
    virtual void SetPixelShader(const D3D12_SHADER_BYTECODE& Binary) override { _PipelineStateStream.PS = Binary; }
    //void SetGeometryShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.GS = Binary; }
    //void SetHullShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.HS = Binary; }
    //void SetDomainShader(const D3D12_SHADER_BYTECODE& Binary) { m_PSODesc.DS = Binary; }

    virtual void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs) override {
      _NumInputElements = numElements;
      _InputElementDescs = inputElementDescs;
    };

    virtual void SetRasterizer(const CD3DX12_RASTERIZER_DESC& rasterizerDesc) override {
      _PipelineStateStream.Rasterizer = rasterizerDesc;
    }

    virtual void SetBlendDescription(const CD3DX12_BLEND_DESC& blendDesc) override {
      _PipelineStateStream.BlendDesc = blendDesc;
    }

  protected:

    const RootSignatureDx12* m_RootSignature;

    ID3D12PipelineState* m_PSO;
    PipelineStateStream _PipelineStateStream;

    UINT _NumInputElements;
    const D3D12_INPUT_ELEMENT_DESC* _InputElementDescs;
    D3D12_RT_FORMAT_ARRAY _RtvFormats;

    size_t _Identifier;

  };

}