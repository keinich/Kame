#pragma once

#include "kmpch.h"

namespace Kame {

  class PipelineStateManager {

    friend class GraphicsPipelineState;

  public: // Functions

    static PipelineStateManager* Get() {
      if (_Instance == nullptr)
        _Instance = new  PipelineStateManager();
      return _Instance;
    }


    void DestroyAll();

  private: // Functions

    inline std::map<size_t, ComPtr<ID3D12PipelineState> >* GetGraphicsPsoHashMap() { return &_GraphicsPSOHashMap; }

  private: // Fields

    static PipelineStateManager* _Instance;

    std::map<size_t, ComPtr<ID3D12PipelineState> > _GraphicsPSOHashMap;
  };

}