#pragma once

#include <d3d12.h> //TODO abstract away D3D12_PIPELINE_STATE_STREAM_DESC

namespace Kame {

  class RenderProgram {

  public:
    virtual void Create(D3D12_PIPELINE_STATE_STREAM_DESC description) = 0;

  };

}