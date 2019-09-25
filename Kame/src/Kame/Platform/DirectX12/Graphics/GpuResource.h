#pragma once

#include "kmpch.h"

namespace Kame {
  
class GpuResource {

  friend class CommandContext;
  friend class GraphicsContext;
  friend class ComputeContext;

public:
  GpuResource() {
    _UsageState = D3D12_RESOURCE_STATE_COMMON;
  };
  ~GpuResource() {};
  
protected:
  ID3D12Resource* _Resource; // TODO in Mini-Engine this is a ComPtr -> why?
  D3D12_RESOURCE_STATES _UsageState;
};

}

