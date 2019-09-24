#include "kmpch.h"
#include "GraphicsContext.h"

namespace Kame {

  void GraphicsContext::ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, float* color) {
    _CommandList->ClearRenderTargetView(rtv, color, 0, nullptr);
  }

}
