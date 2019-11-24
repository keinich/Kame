#pragma once

#include <DirectXMath.h>
#include "Matrix4x4DX.h"
#include "Vector4DX.h"

namespace Kame {

  namespace Math {

    inline Matrix4x4DX XM_CALLCONV MatrixRotationQuaternion(Vector4DX quaternion) {
      return Matrix4x4DX(DirectX::XMMatrixRotationQuaternion(quaternion.GetXmVector()));
    }

    inline Matrix4x4DX XM_CALLCONV MatrixTranspose(Matrix4x4DX m) {
      return Matrix4x4DX(DirectX::XMMatrixTranspose(m.GetXmMatrix()));
    }

    inline Vector4DX XM_CALLCONV Vector3TransformCoord(Vector4DX v, Matrix4x4DX m) {
      return Vector4DX(DirectX::XMVector3Transform(v.GetXmVector(), m.GetXmMatrix()));
    }

  }

}