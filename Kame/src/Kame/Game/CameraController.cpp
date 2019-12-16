#include "kmpch.h"
#include "CameraController.h"

#include <Kame/Graphics/Camera.h>
#include <Kame/Math/MathCommon.h>
#include <Kame\Math\Matrix4x4.h>
#include <Kame/Input/Input.h>

using namespace DirectX;

Kame::CameraController::CameraController(Camera* cameraToControl) :
  _Forward(0),
  _Backward(0),
  _Left(0),
  _Up(0),
  _Down(0),
  _Pitch(0),
  _Yaw(0),
  _Right(0),
  _Shift(false) {
  _ControlledCamera = cameraToControl;

  Input::KeyEvent(Kame::Key::W)->AddHandler("CameraController::Move", BIND_FUNCTION(CameraController::Move));
  Input::KeyEvent(Kame::Key::S)->AddHandler("CameraController::Move", BIND_FUNCTION(CameraController::Move));
  Input::KeyEvent(Kame::Key::A)->AddHandler("CameraController::Move", BIND_FUNCTION(CameraController::Move));
  Input::KeyEvent(Kame::Key::D)->AddHandler("CameraController::Move", BIND_FUNCTION(CameraController::Move));

  Input::MouseMotionEvent()->AddHandler("CameraController::OnMouseMoved", BIND_FUNCTION(CameraController::OnMouseMoved));
  Input::MouseWheelEvent()->AddHandler("CameraController::OnMouseWheel", BIND_FUNCTION(CameraController::OnMouseWheel));
}

Kame::CameraController::~CameraController() {
  Input::KeyEvent(Kame::Key::W)->RemoveHandler("CameraController::Move");
  Input::KeyEvent(Kame::Key::S)->RemoveHandler("CameraController::Move");
  Input::KeyEvent(Kame::Key::A)->RemoveHandler("CameraController::Move");
  Input::KeyEvent(Kame::Key::D)->RemoveHandler("CameraController::Move");

  Input::MouseMotionEvent()->RemoveHandler("CameraController::OnMouseMoved");
  Input::MouseWheelEvent()->RemoveHandler("CameraController::OnMouseWheel");

}

void Kame::CameraController::Update(float elapsedTime) {
  float speedMultipler = (_Shift ? 16.0f : 4.0f);

  XMVECTOR cameraTranslate = XMVectorSet(_Right - _Left, 0.0f, _Forward - _Backward, 1.0f) * speedMultipler * static_cast<float>(elapsedTime);
  XMVECTOR cameraPan = XMVectorSet(0.0f, _Up - _Down, 0.0f, 1.0f) * speedMultipler * static_cast<float>(elapsedTime);
  _ControlledCamera->Translate(cameraTranslate, Space::Local);
  _ControlledCamera->Translate(cameraPan, Space::Local);

  XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(_Pitch), XMConvertToRadians(_Yaw), 0.0f);
  _ControlledCamera->set_Rotation(cameraRotation);

}

bool Kame::CameraController::Move(KeyEventArgs& e) {
  switch (e.Key) {
  case Key::W:
    if (e.State == KeyEventArgs::Pressed)
      _Forward = 1.0f;
    if (e.State == KeyEventArgs::Released)
      _Forward = 0.0f;
    break;
  case Key::S:
    if (e.State == KeyEventArgs::Pressed)
      _Backward = 1.0f;
    if (e.State == KeyEventArgs::Released)
      _Backward = 0.0f;
    break;
  case Key::A:
    if (e.State == KeyEventArgs::Pressed)
      _Left = 1.0f;
    if (e.State == KeyEventArgs::Released)
      _Left = 0.0f;
    break;
  case Key::D:
    if (e.State == KeyEventArgs::Pressed)
      _Right = 1.0f;
    if (e.State == KeyEventArgs::Released)
      _Right = 0.0f;
    break;
  }
  return true;
}

bool Kame::CameraController::OnMouseMoved(MouseMotionEventArgs& e) {
  const float mouseSpeed = 0.1f;
  if (e.LeftButton) {
    _Pitch -= e.RelY * mouseSpeed;

    _Pitch = Math::clamp(_Pitch, -90.0f, 90.0f);

    _Yaw -= e.RelX * mouseSpeed;
  }
  return true;
}

bool Kame::CameraController::OnMouseWheel(MouseWheelEventArgs& e) {
  auto fov = _ControlledCamera->get_FoV();

  fov -= e.WheelDelta;
  fov = Math::clamp(fov, 12.0f, 90.0f);

  _ControlledCamera->set_FoV(fov);

  char buffer[256];
  sprintf_s(buffer, "FoV: %f\n", fov);
  OutputDebugStringA(buffer);
  return true;
}
