#pragma once

#include <DirectXMath.h>

namespace Kame {

  namespace Math {

    using namespace DirectX;

    class Vector4DX {

    public:

      Vector4DX() {
        _XmVector = DirectX::XMVectorSet(0, 0, 0, 0);
      }

      Vector4DX(float x, float y, float z, float w) {
        _XmVector = DirectX::XMVectorSet(x, y, z, w);
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

      Vector4DX& XM_CALLCONV operator = (DirectX::XMVECTOR& other) {
        _XmVector = other;
        return *this;
      }

      Vector4DX XM_CALLCONV operator + (DirectX::XMVECTOR& other) {
        return Vector4DX(_XmVector + other);
      }

      Vector4DX XM_CALLCONV operator += (DirectX::XMVECTOR& other) {
        _XmVector += other;
        return *this;
      }

      Vector4DX XM_CALLCONV operator += (Vector4DX& other) {
        _XmVector += other.GetXmVector();
        return *this;
      }

      Vector4DX XM_CALLCONV operator += (DirectX::FXMVECTOR& other) {
        _XmVector += other;
        return *this;
      }

      Vector4DX XM_CALLCONV operator - () {
        return Vector4DX(-_XmVector);
      }

    protected:
      DirectX::XMVECTOR _XmVector;

    };

  }

}