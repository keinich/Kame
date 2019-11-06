#include "kmpch.h"
#include "GraphicsContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"

namespace Kame {

  void GraphicsContext::ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, float* color) {
    _CommandList->ClearRenderTargetView(rtv, color, 0, nullptr);
  }

  void GraphicsContext::ClearColor(ColorBuffer& target, float* color) {
    _CommandList->ClearRenderTargetView(target.GetRtv(), color, 0, nullptr);
  }

  void GraphicsContext::ClearDepth(DepthBuffer& target) {
    _CommandList->ClearDepthStencilView(target.GetDsv(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
  }

  void GraphicsContext::SetRenderTargets(UINT numRtvs, D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv) {
    _CommandList->OMSetRenderTargets(numRtvs, rtvs, FALSE, &dsv);
  }

}
