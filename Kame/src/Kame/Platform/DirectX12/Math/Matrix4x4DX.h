#pragma once

#include <DirectXMath.h>

namespace Kame {

  namespace Math {

    using namespace DirectX;

    class Matrix4x4DX {

    public:

      //TODO remove this:
      inline XMMATRIX GetXmMatrix() { return _XmMatrix; }

      Matrix4x4DX& operator = (const XMMATRIX& other) {
        _XmMatrix = other;
        return *this;
      }

    protected:
      XMMATRIX _XmMatrix;

    };

  }

}