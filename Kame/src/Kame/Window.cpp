#include "kmpch.h"
#include "Window.h"

#include "Kame/Platform/DirectX12/Graphics/Game.h"

namespace Kame {

  constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

  Window::Window(const std::wstring& name, int width, int height, bool vSync) :
    _hWnd(nullptr),
    _Display(name, width, height, vSync),
    _WindowName(name),
    _PreviousMouseX(0),
    _PreviousMouseY(0),
    _Fullscreen(false),
    _WindowRect() {

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
      return;
    }
    _hWnd = hWnd;
    _Display.Initialize(_hWnd);
  }

  Window::~Window() {
    // Window should be destroyed with Application::DestroyWindow before
    // the window goes out of scope.
    assert(!_hWnd && "Use Application::DestroyWindow before destruction.");
  }

  const std::wstring& Window::GetWindowName() const {
    return _WindowName;
  }

  void Window::Destroy() {
    //m_GUI.Destroy();

    if (auto game = _pGame.lock()) {
      // Notify the registered game that the window is being destroyed.
      game->OnWindowDestroy();
    }

    if (_hWnd) {
      DestroyWindow(_hWnd);
      _hWnd = nullptr;
    }
  }

  void Window::Show() {
    ::ShowWindow(_hWnd, SW_SHOW);
  }

  /**
  * Hide the window.
  */
  void Window::Hide() {
    ::ShowWindow(_hWnd, SW_HIDE);
  }

  bool Window::IsFullScreen() const {
    return _Fullscreen;
  }

  void Window::ToggleFullscreen() {
    SetFullscreen(!_Fullscreen);
  }

  // Set the fullscreen state of the window.
  void Window::SetFullscreen(bool fullscreen) {
    if (_Fullscreen != fullscreen) {
      _Fullscreen = fullscreen;

      if (_Fullscreen) // Switching to fullscreen.
      {
        // Store the current window dimensions so they can be restored 
        // when switching out of fullscreen state.
        ::GetWindowRect(_hWnd, &_WindowRect);

        // Set the window style to a borderless window so the client area fills
        // the entire screen.
        UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        ::SetWindowLongW(_hWnd, GWL_STYLE, windowStyle);

        // Query the name of the nearest display device for the window.
        // This is required to set the fullscreen dimensions of the window
        // when using a multi-monitor setup.
        HMONITOR hMonitor = ::MonitorFromWindow(_hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        ::GetMonitorInfo(hMonitor, &monitorInfo);

        ::SetWindowPos(_hWnd, HWND_TOP,
          monitorInfo.rcMonitor.left,
          monitorInfo.rcMonitor.top,
          monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
          monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
          SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ::ShowWindow(_hWnd, SW_MAXIMIZE);
      }
      else {
        // Restore all the window decorators.
        ::SetWindowLong(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

        ::SetWindowPos(_hWnd, HWND_NOTOPMOST,
          _WindowRect.left,
          _WindowRect.top,
          _WindowRect.right - _WindowRect.left,
          _WindowRect.bottom - _WindowRect.top,
          SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ::ShowWindow(_hWnd, SW_NORMAL);
      }
    }
  }

  void Window::RegisterCallbacks(std::shared_ptr<Game> pGame) {
    _pGame = pGame;

    return;
  }

  void Window::OnUpdate(UpdateEventArgs& e) {
    //m_GUI.NewFrame();

    _UpdateClock.Tick();

    if (auto pGame = _pGame.lock()) {
      UpdateEventArgs updateEventArgs(_UpdateClock.GetDeltaSeconds(), _UpdateClock.GetTotalSeconds(), e.FrameNumber);
      pGame->OnUpdate(updateEventArgs);
    }
  }

  void Window::OnRender(RenderEventArgs& e) {
    _RenderClock.Tick();

    if (auto pGame = _pGame.lock()) {
      RenderEventArgs renderEventArgs(_RenderClock.GetDeltaSeconds(), _RenderClock.GetTotalSeconds(), e.FrameNumber);
      pGame->OnRender(renderEventArgs);
    }
  }

  void Window::OnKeyPressed(KeyEventArgs& e) {
    if (auto pGame = _pGame.lock()) {
      pGame->OnKeyPressed(e);
    }
  }

  void Window::OnKeyReleased(KeyEventArgs& e) {
    if (auto pGame = _pGame.lock()) {
      pGame->OnKeyReleased(e);
    }
  }

  // The mouse was moved
  void Window::OnMouseMoved(MouseMotionEventArgs& e) {
    e.RelX = e.X - _PreviousMouseX;
    e.RelY = e.Y - _PreviousMouseY;

    _PreviousMouseX = e.X;
    _PreviousMouseY = e.Y;

    if (auto game = _pGame.lock()) {
      game->OnMouseMoved(e);
    }
  }

  // A button on the mouse was pressed
  void Window::OnMouseButtonPressed(MouseButtonEventArgs& e) {
    _PreviousMouseX = e.X;
    _PreviousMouseY = e.Y;

    if (auto pGame = _pGame.lock()) {
      pGame->OnMouseButtonPressed(e);
    }
  }

  // A button on the mouse was released
  void Window::OnMouseButtonReleased(MouseButtonEventArgs& e) {
    if (auto pGame = _pGame.lock()) {
      pGame->OnMouseButtonReleased(e);
    }
  }

  // The mouse wheel was moved.
  void Window::OnMouseWheel(MouseWheelEventArgs& e) {
    if (auto pGame = _pGame.lock()) {
      pGame->OnMouseWheel(e);
    }
  }

  void Window::OnResize(ResizeEventArgs& e) {
    _Display.OnResize(e);

    if (auto game = _pGame.lock()) {
      game->OnResize(e);
    }
  }

}
