#pragma once

#include "Event_Cherno.h"

namespace Kame {

  class KAME_API WindowResizedEvent : public ChernoEvent {

    public:
    WindowResizedEvent(unsigned int width, unsigned int height)
      : _Width(width), _Height(height) {
    }

    inline unsigned int GetWidth() const { return _Width; }
    inline unsigned int GetHeight() const { return _Height; }

    std::string ToString() const override {
      std::stringstream ss;
      ss << "WindowsResizeEvent: " << _Width << ", " << _Height;
      return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
      EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
    unsigned int _Width, _Height;

  };

  class KAME_API WindowCloseEvent : public ChernoEvent {
    public:
    WindowCloseEvent() {}

    EVENT_CLASS_TYPE(WindowClose)
      EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };

  class KAME_API AppTickEvent : public ChernoEvent {
    public:
    AppTickEvent() {}

    EVENT_CLASS_TYPE(AppTick)
      EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };

  class KAME_API AppUpdateEvent : public ChernoEvent {
    public:
    AppUpdateEvent() {}

    EVENT_CLASS_TYPE(AppUpdate)
      EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };

  class KAME_API AppRenderEvent : public ChernoEvent {
    public:
    AppRenderEvent() {}

    EVENT_CLASS_TYPE(AppRender)
      EVENT_CLASS_CATEGORY(EventCategoryApplication)
  };

}