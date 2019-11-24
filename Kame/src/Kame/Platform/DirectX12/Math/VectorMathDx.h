#pragma once

#include <DirectXMath.h>
#include "Matrix4x4DX.h"
#include "Vector4DX.h"

#define CALLCONV XM_CALLCONV

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

    inline Vector4DX XM_CALLCONV Vector3Rotate(Vector4DX v, Vector4DX rotationQuaternion) {
      return Vector4DX(DirectX::XMVector3Rotate(v.GetXmVector(), rotationQuaternion.GetXmVector()));
    }

    inline Vector4DX XM_CALLCONV VectorSetW(Vector4DX v, float w) {
      return Vector4DX(DirectX::XMVectorSetW(v.GetXmVector(), w));
    }

    inline Vector4DX  XM_CALLCONV QuaternionMultiply(Vector4DX  q1, Vector4DX  q2) {
      return Vector4DX(DirectX::XMQuaternionMultiply(q1.GetXmVector(), q2.GetXmVector()));
    }

    inline Matrix4x4DX XM_CALLCONV MatrixLookAtLh(
      Vector4DX eyePosition,
      Vector4DX focusPosition,
      Vector4DX upDirection
    ) {
      return Matrix4x4DX(DirectX::XMMatrixLookAtLH(eyePosition.GetXmVector(), focusPosition.GetXmVector(), upDirection.GetXmVector()));
    }

    inline Vector4DX XM_CALLCONV QuaternionRotationMatrix(Matrix4x4DX m) {
      return Vector4DX(DirectX::XMQuaternionRotationMatrix(m.GetXmMatrix()));
    }

    _Use_decl_annotations_
      inline Matrix4x4DX XM_CALLCONV MatrixInverse(Vector4DX* determinant, Matrix4x4DX  m) {
      return Matrix4x4DX(DirectX::XMMatrixInverse(&determinant->GetXmVector(), m.GetXmMatrix()));
    }
    
    inline Matrix4x4DX  XM_CALLCONV MatrixTranslationFromVector(Vector4DX offset) {
      return Matrix4x4DX(DirectX::XMMatrixTranslationFromVector(offset.GetXmVector()));
    }
    
    inline Matrix4x4DX XM_CALLCONV MatrixPerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) {
      return Matrix4x4DX(DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ));
    }

    inline XM_CONSTEXPR float ConvertToRadians(float fDegrees) { return fDegrees * (XM_PI / 180.0f); }

  }

}