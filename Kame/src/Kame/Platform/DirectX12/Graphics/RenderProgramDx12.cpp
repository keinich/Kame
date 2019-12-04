#include "kmpch.h"
#include "DX12Core.h"
#include "RenderProgramDx12.h"
#include "RootSignatureDx12.h"
#include "Kame/Core/Hash.h"
#include <map>
#include <thread>
#include <mutex>
#include <Kame/Core/DebugUtilities.h>

using Kame::Math::IsAligned;
using Microsoft::WRL::ComPtr;
using namespace std;



namespace Kame {

  static map< size_t, ComPtr<ID3D12PipelineState> > s_PSOHashMap;

  void RenderProgramDx12::DestroyAll(void) {
    s_PSOHashMap.clear();
  }

  void RenderProgramDx12::Create() {

    assert(_PipelineStateStream.pRootSignature != nullptr);

    _PipelineStateStream.InputLayout = { nullptr, 0U };

    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
     sizeof(PipelineStateStream), &_PipelineStateStream
    };

    PipelineStateStreamWithoutRtvs pssToHash; 
    ZeroMemory(&pssToHash, sizeof(pssToHash));
    pssToHash.Init(_PipelineStateStream);

    size_t HashCode = Kame::Utility::HashState(&pssToHash);
    HashCode = Kame::Utility::HashState(&_InputElementDescs, _NumInputElements, HashCode);
    HashCode = Kame::Utility::HashState(&_RtvFormats.RTFormats, 1, HashCode);
    _PipelineStateStream.InputLayout = { _InputElementDescs, _NumInputElements };
    _PipelineStateStream.RTVFormats = _RtvFormats;

    ID3D12PipelineState** PSORef = nullptr;
    bool firstCompile = false;
    {
      static mutex s_HashMapMutex;
      lock_guard<mutex> CS(s_HashMapMutex);
      auto iter = s_PSOHashMap.find(HashCode);

      // Reserve space so the next inquiry will find that someone got here first.
      if (iter == s_PSOHashMap.end()) {
        firstCompile = true;
        PSORef = s_PSOHashMap[HashCode].GetAddressOf();
      }
      else
        PSORef = iter->second.GetAddressOf();
    }

    if (firstCompile) {
      ThrowIfFailed(DX12Core::Get().GetDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PSO)));
      s_PSOHashMap[HashCode].Attach(m_PSO);
    }
    else {
      while (*PSORef == nullptr)
        this_thread::yield();
      m_PSO = *PSORef;
    }

  }

  void RenderProgramDx12::SetRootSignature(const RenderProgramSignature* signature) {
    const RootSignatureDx12* rootSignatureDx12 = static_cast<const RootSignatureDx12*>(signature);
    m_RootSignature = rootSignatureDx12;
    _PipelineStateStream.pRootSignature = rootSignatureDx12->GetRootSignature().Get();
  }

}