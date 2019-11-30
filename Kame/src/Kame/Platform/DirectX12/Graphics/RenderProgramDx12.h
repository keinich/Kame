#pragma once

#include <d3d12.h>

#include "RootSignature.h"
//#include "DescriptorAllocation.h"

//#include <cstdint>
namespace Kame {

  class RenderProgramDx12 {

  public:

    virtual const RootSignature& GetRootSignature() const = 0;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const {
      return m_PipelineState;
    }
  protected:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;

  };

}