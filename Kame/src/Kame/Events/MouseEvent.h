#pragma once

#include "Event_Cherno.h"
#include <sstream>

namespace Kame {

class KAME_API MouseMovedEvent : public ChernoEvent {

  public:
  MouseMovedEvent(float x, float y)
    : _MouseX(x), _MouseY(y) {
  }

  inline float GetX() const { return _MouseX; }
  inline float GetY() const { return _MouseY; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseMovedEvent: " << _MouseX << ", " << _MouseY;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
  float _MouseX, _MouseY;

};

class KAME_API MouseScrolledEvent : public ChernoEvent {

  public:
  MouseScrolledEvent(float xOffset, float yOffset)
    : _XOffset(xOffset), _YOffset(yOffset) {
  }

  inline float GetXOffset() const { return _XOffset; }
  inline float GetYOffset() const { return _YOffset; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseScrollEvent: " << _XOffset << ", " << _YOffset;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  private:
  float _XOffset, _YOffset;

};

class KAME_API MouseButtonEvent : public ChernoEvent {

  public:
  inline int GetMouseButton() const { return _Button; }

  EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

  protected:
  MouseButtonEvent(int button)
    : _Button(button) {
  }

  int _Button;
};

class KAME_API MouseButtonPressedEvent : public MouseButtonEvent {

  public:
  MouseButtonPressedEvent(int button)
    : MouseButtonEvent(button) {
  }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonPressedEvent: " << _Button;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonPressed)
};

class KAME_API MouseButtonReleasedEvent : public MouseButtonEvent {

  public:
  MouseButtonReleasedEvent(int button)
    : MouseButtonEvent(button) {
  }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "MouseButtonReleasedEvent: " << _Button;
    return ss.str();
  }

  EVENT_CLASS_TYPE(MouseButtonReleased)
};

}