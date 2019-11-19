#pragma once

#include "kmpch.h"

#include "Kame/Core.h"
#include "Kame/Events/Event.h"

namespace Kame {

  struct WindowProperties {
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProperties(
      const std::string& title = "Kame Engine",
      unsigned int width = 1280,
      unsigned int height = 720
    ) : Title(title), Width(width), Height(height) {
    }

  };

  class KAME_API Display {
  public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Display() {}

    virtual void OnUpdate() = 0;

    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void* GetNativeWindow() const = 0;

    virtual void Show() const = 0;

    static Display* Create(const WindowProperties& props = WindowProperties());
  };

}