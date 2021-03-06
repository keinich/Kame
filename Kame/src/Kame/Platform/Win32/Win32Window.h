#pragma once

//#ifdef KAME_PLATFORM_WIN32

#include <Kame/Application/Window.h>

namespace Kame {

  class Win32Window : public Window {

  public: // Methods

    virtual void PlatformShow() override;
    virtual void PlatformHide() override;

  protected: // Methods
    friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    Win32Window(const std::wstring& name, int width, int height, bool vSync);
    virtual ~Win32Window();

    virtual void PlatformDestroyWindow() override;

    inline HWND GetWindowHandle() { return _NativeWindow; }

    virtual void PlatformCreate(int width, int height, const std::wstring& name) override;
    virtual void PlatformSetFullscreen() override;

  protected: // Fields

  };



  // Old Win32 Window

  //class Win32Window : public Window {

  //public:
  //  Win32Window(const WindowProperties& props);
  //  ~Win32Window();

  //  void OnUpdate() override;

  //  inline unsigned int GetWidth() const override { return _Data.Width; }
  //  inline unsigned int GetHeight() const override { return _Data.Height; }

  //  inline void SetEventCallback(const EventCallbackFn& callback) override { _Data.EventCallback = callback; }
  //  void SetVSync(bool enabled) override;
  //  bool IsVSync() const override;

  //  inline virtual void* GetNativeWindow() const override { return _Window; };
  //  inline HWND GethWnd() const { return _Window; }

  //  void Show() const override;

  //private: // Functions
  //  virtual void Init(const WindowProperties& props);
  //  virtual void Shutdown();

  //  static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

  //private: // Fields
  //  HWND _Window;

  //  struct WindowData {
  //    std::string Title;
  //    unsigned int Width, Height;
  //    bool VSync;

  //    EventCallbackFn EventCallback;
  //  };
  //  
  //  WindowData _Data;
  //};

}

//#endif