#pragma once

namespace Kame {

  class RootSignature;

  class PipelineState {

  public: // Functions

    PipelineState() {
      _Pso = nullptr;
      _RootSignature = nullptr;
    }

    void SetRootSignature(const RootSignature& rootSignature) { _RootSignature = &rootSignature; }

    static void DestroyAll();

    // TODO wieder raus
    ID3D12PipelineState* GetPipelineState() { return _Pso; }

  protected: // Fields

    ID3D12PipelineState* _Pso;

    //ID3D12RootSignature* _RootSignature; // TODO RootSignature Class
    const RootSignature* _RootSignature;

    //static std::map<size_t, ComPtr<ID3D12PipelineState> > s_GraphicsPSOHashMap;
  };

}