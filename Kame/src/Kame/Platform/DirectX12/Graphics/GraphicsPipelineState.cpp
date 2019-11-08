#include "kmpch.h"
#include "GraphicsPipelineState.h"
#include "PipelineStateManager.h"

#include "DX12Core.h"

#include "Kame/Utility/Hash.h"

std::map<size_t, ComPtr<ID3D12PipelineState> > s_GraphicsPSOHashMap;

Kame::GraphicsPipelineState::GraphicsPipelineState() {
  //ZeroMemory(&_PipelineStateStream, sizeof(_PipelineStateStream));
  //_PsoDesc.NodeMask = 1;
  //_PsoDesc.SampleMask = 0xFFFFFFFFu;
  //_PsoDesc.SampleDesc.Count = 1;
  //_PsoDesc.InputLayout.NumElements = 0;
}

void Kame::GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& blendDesc) {
  //_PsoDesc.BlendState = blendDesc;
}

void Kame::GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& rasterizerDesc) {
  //_PsoDesc.DepthStencilState = rasterizerDesc;
}

void Kame::GraphicsPipelineState::SetRasterizerState(D3D12_RASTERIZER_DESC rasterizerDesc) {
  //_PsoDesc.RasterizerState = rasterizerDesc;
}

void Kame::GraphicsPipelineState::SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs) {

  //_PsoDesc.InputLayout.NumElements =numElements;

  if (numElements > 0) {
    D3D12_INPUT_ELEMENT_DESC* NewElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * numElements);
    memcpy(NewElements, inputElementDescs, numElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
    _InputLayouts.reset((const D3D12_INPUT_ELEMENT_DESC*)NewElements);
  }
  else
    _InputLayouts = nullptr;

}

void Kame::GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType) {
  //_PsoDesc.PrimitiveTopologyType = topologyType;
  _PipelineStateStream.PrimitiveTopologyType = topologyType;
}

void Kame::GraphicsPipelineState::SetRenderTargetFormat(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat, UINT MsaaCount, UINT MsaaQualitiy) {
  SetRenderTargetFormats(1, &rtvFormat, dsvFormat, MsaaCount, MsaaQualitiy);
}

void Kame::GraphicsPipelineState::SetRenderTargetFormat(D3D12_RT_FORMAT_ARRAY rtvFormats, DXGI_FORMAT dsvFormat) {
  _PipelineStateStream.RTVFormats = rtvFormats;
  _PipelineStateStream.DSVFormat = dsvFormat;
}

void Kame::GraphicsPipelineState::SetRenderTargetFormats(
  UINT numRtvs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat, UINT MsaaCount, UINT MsaaQuality
) {
  //for (UINT i = 0; i < numRtvs; ++i) {
  //  _PsoDesc.RTVFormats[i] = rtvFormats[i];
  //}
  //for (UINT i = numRtvs; i < _PsoDesc.NumRenderTargets; ++i) {
  //  _PsoDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
  //}
  //_PsoDesc.NumRenderTargets = numRtvs;
  //_PsoDesc.DSVFormat = dsvFormat;
  //_PsoDesc.SampleDesc.Count = MsaaCount;
  //_PsoDesc.SampleDesc.Quality = MsaaQuality;
}

void Kame::GraphicsPipelineState::SetVertexShader(const void* byteCode, size_t size) {
  //_PsoDesc.VS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(byteCode), size);
}

void Kame::GraphicsPipelineState::SetPixelShader(const void* byteCode, size_t size) {
  //_PsoDesc.PS = CD3DX12_SHADER_BYTECODE(const_cast<void*>(byteCode), size);
}

void Kame::GraphicsPipelineState::Finalize() {

  //_PsoDesc.pRootSignature = _RootSignature;
  _PipelineStateStream.pRootSignature = _RootSignature;

  //_PsoDesc.InputLayout.pInputElementDescs = nullptr;
  //_PsoDesc.InputLayout.pInputElementDescs = _InputLayouts.get();
  _PipelineStateStream.InputLayout = {};
  size_t hashCode = Kame::Utility::HashState(&_PipelineStateStream);
  hashCode = Kame::Utility::HashState(_InputLayouts.get(), 2, hashCode);
  _PipelineStateStream.InputLayout = { _InputLayouts.get(), 2 };


  {
    ID3D12PipelineState** psoRef = nullptr;
    bool firstCompile = false;
    static std::mutex hashMapMutex;
    std::lock_guard <std::mutex> cs(hashMapMutex);
    auto graphicsPsoHashMap = *DX12Core::GetPipelineStateManager()->GetGraphicsPsoHashMap();
    //auto graphicsPsoHashMap = s_GraphicsPSOHashMap;
    
    auto entry = graphicsPsoHashMap.find(hashCode);

    if (entry == graphicsPsoHashMap.end()) {
      firstCompile = true;
      psoRef = graphicsPsoHashMap[hashCode].GetAddressOf();
    }
    else {
      psoRef = entry->second.GetAddressOf();
    }

    if (firstCompile) {
      D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &_PipelineStateStream };
      ThrowIfFailed(DX12Core::GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&_Pso)));
      graphicsPsoHashMap[hashCode].Attach(_Pso);
    }
    else {
      while (*psoRef == nullptr)
        std::this_thread::yield();
      _Pso = *psoRef;
    }

  }

  // TODO in Mini Engine ganz viel

  //ThrowIfFailed(DX12Core::GetDevice()->CreateGraphicsPipelineState(&_PsoDesc, IID_PPV_ARGS(&_Pso)));
}

void Kame::GraphicsPipelineState::DestroyAll() {
  s_GraphicsPSOHashMap.clear();
}
