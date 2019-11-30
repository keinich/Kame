#pragma once

#include <d3d12.h>

namespace Kame {

  namespace GraphicsCommon {


    extern D3D12_RASTERIZER_DESC RasterizerDefault;

    void InitializeCommonState();
    void DestroyCommonState();

  }

}