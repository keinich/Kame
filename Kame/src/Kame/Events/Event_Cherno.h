#pragma once

#include "kmpch.h"
#include <Kame/Memory/Memory.h>

namespace Kame {

  enum class EventType {
    None = 0,
    WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
    AppTick, AppUpdate, AppRender,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
  };

  enum EventCategory {
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput = BIT(1),
    EventCategoryKeybord = BIT(2),
    EventCategoryMouse = BIT(3),
    EventCategoryMouseButton = BIT(4)
  };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
                                virtual EventType GetEventType() const override { return GetStaticType(); } \
                                virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

  class KAME_API ChernoEvent {
    friend class EventDispatcher;

    public:
    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
    inline bool IsHandled() { return _Handled; }

    protected:
    bool _Handled = false;
  };

  class EventDispatcher {
    template<typename T>
    using EventFn = std::function<bool(T&)>;

    public:
    EventDispatcher(ChernoEvent& event) : _Event(event) {}

    template<typename T>
    bool Dispatch(EventFn<T> func) {
      if (_Event.GetEventType() == T::GetStaticType()) {
        _Event._Handled = func(*(T*)&_Event);
        return true;
      }
      return false;
    }

    private:
      ChernoEvent& _Event;
  };

  inline std::ostream& operator<<(std::ostream& os, const ChernoEvent& e) {
    return os << e.ToString();
  }

}
