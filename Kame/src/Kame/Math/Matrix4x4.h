#pragma once

namespace Kame {

  namespace Math {

    //#ifdef KAME_PLATFORM_DIRECTX12
#include "Kame/Platform/DirectX12/Math/Matrix4x4DX.h"
    typedef Kame::Math::Matrix4x4DX Matrix4x4;
    //#endif

        //class Vector4 {

        //public:

        //};

  }

}