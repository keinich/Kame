#pragma once

#include "kmpch.h"

namespace Kame {

  class PipelineStateManager {

  public: // Functions

    static PipelineStateManager* Get() {
      if (_Instance == nullptr)
        _Instance = new  PipelineStateManager();
      return _Instance;
    }

    inline std::map<size_t, ComPtr<ID3D12PipelineState> >* GetGraphicsPsoHashMap() { return &_GraphicsPSOHashMap; }

    void DestroyAll();

  private: // Fields

    static PipelineStateManager* _Instance;

    std::map<size_t, ComPtr<ID3D12PipelineState> > _GraphicsPSOHashMap;
  };

}