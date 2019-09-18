#pragma once

#ifdef KAME_PLATFORM_WIN32

#include "Kame/Window.h"

namespace Kame {

  class Win32Window : public Window {

  public:
    Win32Window(const WindowProperties& props);
    ~Win32Window();

    void OnUpdate() override;

    inline unsigned int GetWidth() const override { return _Data.Width; }
    inline unsigned int GetHeight() const override { return _Data.Height; }

    inline void SetEventCallback(const EventCallbackFn& callback) override { _Data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    inline virtual void* GetNativeWindow() const override { return _Window; };
    inline HWND GethWnd() const { return *_Window; }

  private: // Functions
    virtual void Init(const WindowProperties& props);
    virtual void Shutdown();

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  private: // Fields
    HWND* _Window;

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