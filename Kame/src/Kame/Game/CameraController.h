#pragma once

#include <Kame/Events/EventArgs.h>
#include <Kame\Events\Event.h>

namespace Kame {

  class Camera;

  class CameraController {

  public:
    CameraController(Camera* cameraToControl);
    virtual ~CameraController();

    void Update(float elapsedTime);

  private: //Methods

    bool Move(KeyEventArgs& e);
    bool OnMouseMoved(MouseMotionEventArgs& e);
    bool OnMouseWheel(MouseWheelEventArgs& e);

  private: //Fields
    Camera* _ControlledCamera;

    float _Forward;
    float _Backward;
    float _Left;
    float _Right;
    float _Up;
    float _Down;

    float _Pitch;
    float _Yaw;

    bool _Shift;

    //TODO this is ugly, identify EventHandlers better!
    //EventHandle _HandleForward;
  };

}