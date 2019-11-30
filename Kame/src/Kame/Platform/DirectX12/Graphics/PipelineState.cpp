//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard

#include "kmpch.h"
#include "DX12Core.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "Kame/Utility/Hash.h"
#include <map>
#include <thread>
#include <mutex>

using Kame::Math::IsAligned;
//using namespace Graphics;
using Microsoft::WRL::ComPtr;
using namespace std;

namespace Kame {

  static map< size_t, ComPtr<ID3D12PipelineState> > s_GraphicsPSOHashMap;
  static map< size_t, ComPtr<ID3D12PipelineState> > s_ComputePSOHashMap;

  void PSO::DestroyAll(void) {
    s_GraphicsPSOHashMap.clear();
    s_ComputePSOHashMap.clear();
  }


  GraphicsPSO::GraphicsPSO() {
    ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
    m_PSODesc.NodeMask = 1;
    m_PSODesc.SampleMask = 0xFFFFFFFFu;
    m_PSODesc.SampleDesc.Count = 1;
    m_PSODesc.InputLayout.NumElements = 0;
  }

  void GraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& BlendDesc) {
    m_PSODesc.BlendState = BlendDesc;
  }

  void GraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc) {
    m_PSODesc.RasterizerState = RasterizerDesc;
  }

  void GraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc) {
    m_PSODesc.DepthStencilState = DepthStencilDesc;
  }

  void GraphicsPSO::SetSampleMask(UINT SampleMask) {
    m_PSODesc.SampleMask = SampleMask;
  }

  void GraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType) {
    assert(TopologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, "Can't draw with undefined topology");
    m_PSODesc.PrimitiveTopologyType = TopologyType;
  }

  void GraphicsPSO::SetPrimitiveRestart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBProps) {
    m_PSODesc.IBStripCutValue = IBProps;
  }

  void GraphicsPSO::SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality) {
    SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
  }

  void GraphicsPSO::SetRenderTargetFormats(UINT NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, UINT MsaaCount, UINT MsaaQuality) {
    assert(NumRTVs == 0 || RTVFormats != nullptr, "Null format array conflicts with non-zero length");
    for (UINT i = 0; i < NumRTVs; ++i)
      m_PSODesc.RTVFormats[i] = RTVFormats[i];
    for (UINT i = NumRTVs; i < m_PSODesc.NumRenderTargets; ++i)
      m_PSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
    m_PSODesc.NumRenderTargets = NumRTVs;
    m_PSODesc.DSVFormat = DSVFormat;
    m_PSODesc.SampleDesc.Count = MsaaCount;
    m_PSODesc.SampleDesc.Quality = MsaaQuality;
  }

  void GraphicsPSO::SetRenderTargetFormats(D3D12_RT_FORMAT_ARRAY rtvFormats, DXGI_FORMAT dsvFormat, UINT MsaaCount, UINT MsaaQuality) {
    UINT numRtvs = rtvFormats.NumRenderTargets;
    const DXGI_FORMAT* x  = rtvFormats.RTFormats;
    SetRenderTargetFormats(numRtvs, x, dsvFormat, MsaaCount, MsaaQuality);
  }

  void GraphicsPSO::SetInputLayout(UINT NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs) {
    m_PSODesc.InputLayout.NumElements = NumElements;

    if (NumElements > 0) {
      D3D12_INPUT_ELEMENT_DESC* NewElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * NumElements);
      memcpy(NewElements, pInputElementDescs, NumElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
      m_InputLayouts.reset((const D3D12_INPUT_ELEMENT_DESC*)NewElements);
    }
    else
      m_InputLayouts = nullptr;
  }

  void GraphicsPSO::Finalize() {
    // Make sure the root signature is finalized first
    m_PSODesc.pRootSignature = m_RootSignature->GetRootSignature().Get();
    assert(m_PSODesc.pRootSignature != nullptr);

    m_PSODesc.InputLayout.pInputElementDescs = nullptr;
    size_t HashCode = Kame::Utility::HashState(&m_PSODesc);
    HashCode = Kame::Utility::HashState(m_InputLayouts.get(), m_PSODesc.InputLayout.NumElements, HashCode);
    m_PSODesc.InputLayout.pInputElementDescs = m_InputLayouts.get();

    ID3D12PipelineState** PSORef = nullptr;
    bool firstCompile = false;
    {
      static mutex s_HashMapMutex;
      lock_guard<mutex> CS(s_HashMapMutex);
      auto iter = s_GraphicsPSOHashMap.find(HashCode);

      // Reserve space so the next inquiry will find that someone got here first.
      if (iter == s_GraphicsPSOHashMap.end()) {
        firstCompile = true;
        PSORef = s_GraphicsPSOHashMap[HashCode].GetAddressOf();
      }
      else
        PSORef = iter->second.GetAddressOf();
    }

    if (firstCompile) {
      ThrowIfFailed(DX12Core::Get().GetDevice()->CreateGraphicsPipelineState(&m_PSODesc, MY_IID_PPV_ARGS(&m_PSO)));
      s_GraphicsPSOHashMap[HashCode].Attach(m_PSO);
    }
    else {
      while (*PSORef == nullptr)
        this_thread::yield();
      m_PSO = *PSORef;
    }
  }

  void PSO::Create() {
    //ThrowIfFailed(DX12Core::Get().GetDevice()->CreatePipelineState(&description, IID_PPV_ARGS(&m_PSO)));
    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
       sizeof(PipelineStateStream), & _PipelineStateStream
    };
    ThrowIfFailed(DX12Core::Get().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PSO)));
  }

  void PSO::SetRootSignature(const RootSignature& BindMappings) {
    m_RootSignature = &BindMappings;
    _PipelineStateStream.pRootSignature = BindMappings.GetRootSignature().Get();
  }

  void ComputePSO::Finalize() {
    // Make sure the root signature is finalized first
    m_PSODesc.pRootSignature = m_RootSignature->GetRootSignature().Get();
    assert(m_PSODesc.pRootSignature != nullptr);

    size_t HashCode = Kame::Utility::HashState(&m_PSODesc);

    ID3D12PipelineState** PSORef = nullptr;
    bool firstCompile = false;
    {
      static mutex s_HashMapMutex;
      lock_guard<mutex> CS(s_HashMapMutex);
      auto iter = s_ComputePSOHashMap.find(HashCode);

      // Reserve space so the next inquiry will find that someone got here first.
      if (iter == s_ComputePSOHashMap.end()) {
        firstCompile = true;
        PSORef = s_ComputePSOHashMap[HashCode].GetAddressOf();
      }
      else
        PSORef = iter->second.GetAddressOf();
    }

    if (firstCompile) {
      ThrowIfFailed(Kame::DX12Core::Get().GetDevice()->CreateComputePipelineState(&m_PSODesc, MY_IID_PPV_ARGS(&m_PSO)));
      s_ComputePSOHashMap[HashCode].Attach(m_PSO);
    }
    else {
      while (*PSORef == nullptr)
        this_thread::yield();
      m_PSO = *PSORef;
    }
  }

  ComputePSO::ComputePSO() {
    ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
    m_PSODesc.NodeMask = 1;
  }

}