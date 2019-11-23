#include "kmpch.h"
#include "Win32Window.h"
#include "Kame/Application.h"


namespace Kame {

  constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

  Win32Window::Win32Window(const std::wstring& name, int width, int height, bool vSync) :
    Window(name, width, height, vSync) {
    PlatformCreate(width, height, name);
    _Display.Initialize(_NativeWindow);
  }

  Win32Window::~Win32Window() {
    // Window should be destroyed with Application::DestroyWindow before
    // the window goes out of scope.
    assert(!_NativeWindow && "Use Application::DestroyWindow before destruction.");
  }

  void Win32Window::PlatformDestroyWindow() {
    DestroyWindow(_NativeWindow);
  }

  void Win32Window::PlatformShow() {
    ::ShowWindow(_NativeWindow, SW_SHOW);
  }

  void Win32Window::PlatformHide() {
    ::ShowWindow(_NativeWindow, SW_HIDE);
  }

  void Win32Window::PlatformCreate(int width, int height, const std::wstring& name) {
    HINSTANCE hInstance = GetModuleHandle(NULL); //TODO think about if there is a situation where this is not correct        

    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = CreateWindowW(
      WINDOW_CLASS_NAME, name.c_str(),
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd) {
      MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
      throw std::exception("Could not create the render window");
    }
    _NativeWindow = hWnd;
  }

  void Win32Window::PlatformSetFullscreen() {
    if (_Fullscreen) // Switching to fullscreen.
    {
      // Store the current window dimensions so they can be restored 
      // when switching out of fullscreen state.
      ::GetWindowRect(_NativeWindow, &_WindowRect);

      // Set the window style to a borderless window so the client area fills
      // the entire screen.
      UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

      ::SetWindowLongW(_NativeWindow, GWL_STYLE, windowStyle);

      // Query the name of the nearest display device for the window.
      // This is required to set the fullscreen dimensions of the window
      // when using a multi-monitor setup.
      HMONITOR hMonitor = ::MonitorFromWindow(_NativeWindow, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitorInfo = {};
      monitorInfo.cbSize = sizeof(MONITORINFOEX);
      ::GetMonitorInfo(hMonitor, &monitorInfo);

      ::SetWindowPos(_NativeWindow, HWND_TOP,
        monitorInfo.rcMonitor.left,
        monitorInfo.rcMonitor.top,
        monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
        monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
        SWP_FRAMECHANGED | SWP_NOACTIVATE);

      ::ShowWindow(_NativeWindow, SW_MAXIMIZE);
    }
    else {
      // Restore all the window decorators.
      ::SetWindowLong(_NativeWindow, GWL_STYLE, WS_OVERLAPPEDWINDOW);

      ::SetWindowPos(_NativeWindow, HWND_NOTOPMOST,
        _WindowRect.left,
        _WindowRect.top,
        _WindowRect.right - _WindowRect.left,
        _WindowRect.bottom - _WindowRect.top,
        SWP_FRAMECHANGED | SWP_NOACTIVATE);

      ::ShowWindow(_NativeWindow, SW_NORMAL);
    }
  }

}