#pragma once

#include <DirectXMath.h>

namespace Kame {

  namespace Math {

    using namespace DirectX;

    class Matrix4x4DX {

    public:

      Matrix4x4DX(XMMATRIX xmMatrix) : _XmMatrix(xmMatrix) {}

      //TODO remove this:
      inline XMMATRIX GetXmMatrix() { return _XmMatrix; }

      Matrix4x4DX& XM_CALLCONV operator = (const XMMATRIX& other) {
        _XmMatrix = other;
        return *this;
      }

      Matrix4x4DX XM_CALLCONV operator * (Matrix4x4DX& const other) {
        return Matrix4x4DX(_XmMatrix * other.GetXmMatrix());
      }

    protected:
      XMMATRIX _XmMatrix;

    };

  }

}