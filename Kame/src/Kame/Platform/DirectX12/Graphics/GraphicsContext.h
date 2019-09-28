#pragma once

#include "CommandContext.h"

namespace Kame {

  class GraphicsContext : public CommandContext {

  public:

    static GraphicsContext& Begin(const std::wstring& ID = L"") { return CommandContext::Begin(ID).GetGraphicsContext(); }

    void ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, float* color);
    void ClearColor(class ColorBuffer& target, float* color);

  };

}