#include "kmpch.h"
#include "Window.h"

#include "Kame/Platform/DirectX12/Graphics/Game.h"

namespace Kame {

  Window::Window(HWND hWnd, const std::wstring& name, int width, int height, bool vSync) :
    _Display(hWnd, name, width, height, vSync),
    _hWnd(hWnd),
    _WindowName(name),
    _PreviousMouseX(0),
    _PreviousMouseY(0) {}

  Window::~Window() {}

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
