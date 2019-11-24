#pragma once

#include <DirectXMath.h>

namespace Kame {

  namespace Math {

    class Vector4DX {

    public:
      Vector4DX(float x, float y, float z, float w) {
        _XmVector = DirectX::XMVectorSet(0, 5, -20, 1);
      }

    protected:
      DirectX::XMVECTOR _XmVector;

    };

  }

}