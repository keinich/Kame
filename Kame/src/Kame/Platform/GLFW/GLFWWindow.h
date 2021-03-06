#pragma once

#ifdef KAME_PLATFORM_GLFW

#include "Kame/Window.h"

#include "GLFW/glfw3.h"

namespace Kame {

  class GLFWWindow : public Window {

  public:
    GLFWWindow(const WindowProperties& props);
    ~GLFWWindow();

    void OnUpdate() override;

    inline unsigned int GetWidth() const override { return _Data.Width; }
    inline unsigned int GetHeight() const override { return _Data.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override { _Data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    inline virtual void* GetNativeWindow() const override { return _Window; };

  private: // Functions
    virtual void Init(const WindowProperties& props);
    virtual void Shutdown();

  private: // Fields
    struct GLFWwindow* _Window;

    struct WindowData {
      std::string Title;
      unsigned int Width, Height;
      bool VSync;

      EventCallbackFn EventCallback;
    };
    
    WindowData _Data;
  };

}

#endif