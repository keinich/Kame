#pragma once

#include "Kame/Input.h"

namespace Kame {
  
  class GLFWInput : public Input {

  protected:

    virtual bool IsKeyPressedImpl(int keycode) override;

    virtual bool IsMouseButtonPressedImpl(int button) ;
    virtual float GetMouseXImpl();
    virtual float GetMouseYImpl();
    virtual std::pair<float, float> GetMousePositionImpl();


  };

}