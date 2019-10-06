#pragma once

#include <DirectXMath.h>

namespace Kame {

  class Vector3 {

  public:
    inline Vector3(float x, float y, float z) { _Vector = DirectX::XMVectorSet(x, y, z, z); }

  protected:
    DirectX::XMVECTOR _Vector;

  };

}