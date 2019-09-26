#pragma once

#include "kmpch.h"

namespace Kame {
  
class GpuResource {

  friend class CommandContext;
  friend class GraphicsContext;
  friend class ComputeContext;

public:
  GpuResource() {
    _Resource = nullptr;
    _UsageState = D3D12_RESOURCE_STATE_COMMON;
  };
  ~GpuResource() {};
  
  void Destroy();

protected:
  ID3D12Resource* _Resource; // TODO in Mini-Engine this is a ComPtr -> why?
  //ComPtr<ID3D12Resource> _Resource1; // TODO in Mini-Engine this is a ComPtr -> why?
  D3D12_RESOURCE_STATES _UsageState;
};

}

