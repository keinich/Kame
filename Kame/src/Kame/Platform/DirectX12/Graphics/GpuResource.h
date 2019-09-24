#pragma once

#include "kmpch.h"

namespace Kame {
  
class GpuResource {

public:
  GpuResource();
  ~GpuResource();


private:
  ID3D12Resource* _Resource;
};

}

