#pragma once

#include "kmpch.h"

#include "Kame/Core.h"
#include "Kame/Events/Event.h"

#include "Kame/Platform/DirectX12/Graphics/Display.h"

namespace Kame {

  class Window {

  public:

    // TODO : make this protected
    Display& GetDisplay() { return _Display; }

    const std::wstring& GetWindowName() const;

  protected: // Methods

    // The Window procedure needs to call protected methods of this class.
    friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    friend class Application;
    friend class Game;

    Window() = delete;
    Window(HWND hWnd, const std::wstring& name, int width, int height, bool vSync);
    virtual ~Window();

    /**
    * Destroy this window.
    */
    void Destroy();

    // Register a Game with this window. This allows
    // the window to callback functions in the Game class.
    void RegisterCallbacks(std::shared_ptr<Game> pGame);

    // Update and Draw can only be called by the application.
    virtual void OnUpdate(UpdateEventArgs& e);
    virtual void OnRender(RenderEventArgs& e);

    // A keyboard key was pressed
    virtual void OnKeyPressed(KeyEventArgs& e);
    // A keyboard key was released
    virtual void OnKeyReleased(KeyEventArgs& e);

    // The mouse was moved
    virtual void OnMouseMoved(MouseMotionEventArgs& e);
    // A button on the mouse was pressed
    virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
    // A button on the mouse was released
    virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
    // The mouse wheel was moved.
    virtual void OnMouseWheel(MouseWheelEventArgs& e);

    virtual void OnResize(ResizeEventArgs& e);

  protected: // Fields

    Display _Display; // TODO can a Window contain multiple Displays?

    HWND _hWnd;

    int _PreviousMouseX;
    int _PreviousMouseY;

    std::weak_ptr<Game> _pGame;

    HighResolutionClock _UpdateClock;
    HighResolutionClock _RenderClock;

    std::wstring _WindowName;

  };

  struct WindowProperties {
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProperties(
      const std::string& title = "Kame Engine",
      unsigned int width = 1280,
      unsigned int height = 720
    ) : Title(title), Width(width), Height(height) {}

  };

  class KAME_API DisplayOld {
  public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~DisplayOld() {}

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