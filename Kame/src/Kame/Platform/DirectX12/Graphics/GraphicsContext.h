#pragma once

#include "CommandContext.h"

namespace Kame {

  class GraphicsContext : public CommandContext {

  public:

    static GraphicsContext& Begin(const std::wstring& ID = L"") { return CommandContext::Begin(ID).GetGraphicsContext(); }

    void ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, float* color);
    void ClearColor(class ColorBuffer& target, float* color);
    void ClearDepth(class DepthBuffer& target);

    void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { SetRenderTargets(1, &rtv, dsv); }
    void SetRenderTargets(UINT numRtvs, D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv);


  };

}