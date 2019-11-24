#include "kmpch.h"
#include "Camera.h"
#include "Kame/Math/VectorMath.h"

//using namespace DirectX;

namespace Kame {

using namespace Math;

  Camera::Camera()
    : _ViewDirty(true)
    , _InverseViewDirty(true)
    , _ProjectionDirty(true)
    , _InverseProjectionDirty(true)
    , _vFoV(45.0f)
    , _AspectRatio(1.0f)
    , _zNear(0.1f)
    , _zFar(100.0f) {
    pData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
    pData->_Translation = Kame::Math::Vector4::VectorZero();
    pData->_Rotation = Kame::Math::Vector4::QuaternionIdentity();
  }

  Camera::~Camera() {
    _aligned_free(pData);
  }

  void CALLCONV Camera::set_LookAt(Vector4 eye, Vector4 target, Vector4 up) {
    pData->_ViewMatrix = Kame::Math::MatrixLookAtLh(eye, target, up);

    pData->_Translation = eye;
    pData->_Rotation = Kame::Math::QuaternionRotationMatrix(Kame::Math::MatrixTranspose(pData->_ViewMatrix));

    _InverseViewDirty = true;
    _ViewDirty = false;
  }

  Matrix4x4 Camera::get_ViewMatrix() const {
    if (_ViewDirty) {
      UpdateViewMatrix();
    }
    return pData->_ViewMatrix;
  }

  Matrix4x4 Camera::get_InverseViewMatrix() const {
    if (_InverseViewDirty) {
      pData->_InverseViewMatrix = Kame::Math::MatrixInverse(nullptr, pData->_ViewMatrix);
      _InverseViewDirty = false;
    }

    return pData->_InverseViewMatrix;
  }

  void Camera::set_Projection(float fovy, float aspect, float zNear, float zFar) {
    _vFoV = fovy;
    _AspectRatio = aspect;
    _zNear = zNear;
    _zFar = zFar;

    _ProjectionDirty = true;
    _InverseProjectionDirty = true;
  }

  Matrix4x4 Camera::get_ProjectionMatrix() const {
    if (_ProjectionDirty) {
      UpdateProjectionMatrix();
    }

    return pData->_ProjectionMatrix;
  }

  Matrix4x4 Camera::get_InverseProjectionMatrix() const {
    if (_InverseProjectionDirty) {
      UpdateInverseProjectionMatrix();
    }

    return pData->_InverseProjectionMatrix;
  }

  void Camera::set_FoV(float fovy) {
    if (_vFoV != fovy) {
      _vFoV = fovy;
      _ProjectionDirty = true;
      _InverseProjectionDirty = true;
    }
  }

  float Camera::get_FoV() const {
    return _vFoV;
  }


  void CALLCONV Camera::set_Translation(Vector4 translation) {
    pData->_Translation = translation;
    _ViewDirty = true;
  }

  Vector4 Camera::get_Translation() const {
    return pData->_Translation;
  }

  void Camera::set_Rotation(Vector4 rotation) {
    pData->_Rotation = rotation;
  }

  Vector4 Camera::get_Rotation() const {
    return pData->_Rotation;
  }

  void CALLCONV Camera::Translate(Vector4 translation, Space space) {
    switch (space) {
    case Space::Local:
    {
      pData->_Translation += Kame::Math::Vector3Rotate(translation, pData->_Rotation);
    }
    break;
    case Space::World:
    {
      pData->_Translation += translation;
    }
    break;
    }

    pData->_Translation = Kame::Math::VectorSetW(pData->_Translation, 1.0f);

    _ViewDirty = true;
    _InverseViewDirty = true;
  }

  void Camera::Rotate(Vector4 quaternion) {
    pData->_Rotation = Kame::Math::QuaternionMultiply(pData->_Rotation, quaternion);

    _ViewDirty = true;
    _InverseViewDirty = true;
  }

  void Camera::UpdateViewMatrix() const {
    Matrix4x4 rotationMatrix = MatrixTranspose(MatrixRotationQuaternion(pData->_Rotation));
    Matrix4x4 translationMatrix = MatrixTranslationFromVector(-(pData->_Translation));

    pData->_ViewMatrix = translationMatrix * rotationMatrix;

    _InverseViewDirty = true;
    _ViewDirty = false;
  }

  void Camera::UpdateInverseViewMatrix() const {
    if (_ViewDirty) {
      UpdateViewMatrix();
    }

    pData->_InverseViewMatrix = MatrixInverse(nullptr, pData->_ViewMatrix);
    _InverseViewDirty = false;
  }

  void Camera::UpdateProjectionMatrix() const {
    pData->_ProjectionMatrix = MatrixPerspectiveFovLH(ConvertToRadians(_vFoV), _AspectRatio, _zNear, _zFar);

    _ProjectionDirty = false;
    _InverseProjectionDirty = true;
  }

  void Camera::UpdateInverseProjectionMatrix() const {
    if (_ProjectionDirty) {
      UpdateProjectionMatrix();
    }

    pData->_InverseProjectionMatrix = MatrixInverse(nullptr, pData->_ProjectionMatrix);
    _InverseProjectionDirty = false;
  }

}