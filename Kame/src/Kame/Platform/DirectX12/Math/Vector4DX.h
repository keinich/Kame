#pragma once

#include <DirectXMath.h>

namespace Kame {

  namespace Math {

    using namespace DirectX;

    class Vector4DX {

    public:
      Vector4DX(float x, float y, float z, float w) {
        _XmVector = DirectX::XMVectorSet(0, 5, -20, 1);
      }

      Vector4DX(DirectX::XMVECTOR xmVector) {
        _XmVector = xmVector;
      }

      static inline Vector4DX XM_CALLCONV VectorZero() {
        return DirectX::XMVectorZero();
      }

      static inline Vector4DX XM_CALLCONV QuaternionIdentity() {
        return DirectX::XMQuaternionIdentity();
      }


      //TODO Remove this
      DirectX::XMVECTOR GetXmVector() { return _XmVector; }

      Vector4DX& XM_CALLCONV operator = (DirectX::XMVECTOR& const other) {
        _XmVector = other;
        return *this;
      }

      Vector4DX XM_CALLCONV operator + (DirectX::XMVECTOR& const other) {
        return Vector4DX(_XmVector + other);
      }

      Vector4DX XM_CALLCONV operator += (DirectX::XMVECTOR& const other) {
        _XmVector += other;
        return *this;
      }

      Vector4DX XM_CALLCONV operator += (DirectX::FXMVECTOR& const other) {
        _XmVector += other;
        return *this;
      }

    protected:
      DirectX::XMVECTOR _XmVector;

    };

  }

}