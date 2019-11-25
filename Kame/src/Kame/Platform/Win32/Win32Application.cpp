#include "kmpch.h"

#include "Kame/Application/Application.h"
#include "Kame/Events/EventArgs.h"
#include "Kame/Events/KeyEvent.h"
#include "Win32Window.h"
#include "Kame/Input/Input.h"

namespace Kame {

  constexpr wchar_t WINDOW_CLASS_NAME[] = L"DX12RenderWindowClass";

  // Convert the message ID into a MouseButton ID
  MouseButtonEventArgs::MouseButton DecodeMouseButton2(UINT messageID) {
    MouseButtonEventArgs::MouseButton mouseButton = MouseButtonEventArgs::None;
    switch (messageID) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    {
      mouseButton = MouseButtonEventArgs::Left;
    }
    break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
    {
      mouseButton = MouseButtonEventArgs::Right;
    }
    break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MBUTTONDBLCLK:
    {
      mouseButton = MouseButtonEventArgs::Middel;
    }
    break;
    }

    return mouseButton;
  }

  static LRESULT CALLBACK WndProc(KAME_NATIVE_WINDOW hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  void Application::PlatformInitialize() {
    // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
    // Using this awareness context allows the client area of the window 
    // to achieve 100% scaling while still allowing non-client window content to 
    // be rendered in a DPI sensitive fashion.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEXW wndClass = { 0 };

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    //wndClass.hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE( APP_ICON ) );
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = WINDOW_CLASS_NAME;
    //wndClass.hIconSm = LoadIcon( m_hInstance, MAKEINTRESOURCE( APP_ICON ) );

    if (!RegisterClassExW(&wndClass)) {
      MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
    }
  }

  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    //if ( ImGui_ImplWin32_WndProcHandler( hwnd, message, wParam, lParam ) )
    //{
    //    return true;
    //}

    std::shared_ptr<Win32Window> pWindow = Application::GetWindow(hWnd);




    if (pWindow) {
      switch (message) {
      case WM_PAINT:
      {
        Application::Get().IncrementFrameCount(); // TODO is this correct with multiple windows?

        // Delta time will be filled in by the Window.
        UpdateEventArgs updateEventArgs(0.0f, 0.0f, Application::Get().GetFrameCount());
        pWindow->OnUpdate(updateEventArgs);
        RenderEventArgs renderEventArgs(0.0f, 0.0f, Application::Get().GetFrameCount());
        // Delta time will be filled in by the Window.
        pWindow->OnRender(renderEventArgs);
      }
      break;
      case WM_SYSKEYDOWN:
      case WM_KEYDOWN:
      {
        MSG charMsg;
        // Get the Unicode character (UTF-16)
        unsigned int c = 0;
        // For printable characters, the next message will be WM_CHAR.
        // This message contains the character code we need to send the KeyPressed event.
        // Inspired by the SDL 1.2 implementation.
        if (PeekMessageW(&charMsg, hWnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR) {
          GetMessage(&charMsg, hWnd, 0, 0);
          c = static_cast<unsigned int>(charMsg.wParam);

          //if ( charMsg.wParam > 0 && charMsg.wParam < 0x10000 )
          //    ImGui::GetIO().AddInputCharacter( (unsigned short)charMsg.wParam );
        }
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        Kame::Key key = (Kame::Key)wParam;
        unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
        KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Pressed, shift, control, alt);
        pWindow->OnKeyPressed(keyEventArgs);
        Input::KeyEvent(key)->Raise(keyEventArgs);



        //KeyPressedEvent event(wParam, 0);
        //pWindow->OnKeyPressed(event);

      }
      break;
      case WM_SYSKEYUP:
      case WM_KEYUP:
      {
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        Kame::Key key = (Kame::Key)wParam;
        unsigned int c = 0;
        unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

        // Determine which key was released by converting the key code and the scan code
        // to a printable character (if possible).
        // Inspired by the SDL 1.2 implementation.
        unsigned char keyboardState[256];
        GetKeyboardState(keyboardState);
        wchar_t translatedCharacters[4];
        if (int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0) {
          c = translatedCharacters[0];
        }

        KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Released, shift, control, alt);
        pWindow->OnKeyReleased(keyEventArgs);
        Input::KeyEvent(key)->Raise(keyEventArgs);
      }
      break;
      // The default window procedure will play a system notification sound 
      // when pressing the Alt+Enter keyboard combination if this message is 
      // not handled.
      case WM_SYSCHAR:
        break;
      case WM_MOUSEMOVE:
      {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = ((int)(short)LOWORD(lParam));
        int y = ((int)(short)HIWORD(lParam));

        MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
        pWindow->OnMouseMoved(mouseMotionEventArgs);
      }
      break;
      case WM_LBUTTONDOWN:
      case WM_RBUTTONDOWN:
      case WM_MBUTTONDOWN:
      {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = ((int)(short)LOWORD(lParam));
        int y = ((int)(short)HIWORD(lParam));

        MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton2(message), MouseButtonEventArgs::Pressed, lButton, mButton, rButton, control, shift, x, y);
        pWindow->OnMouseButtonPressed(mouseButtonEventArgs);
      }
      break;
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      case WM_MBUTTONUP:
      {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift = (wParam & MK_SHIFT) != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = ((int)(short)LOWORD(lParam));
        int y = ((int)(short)HIWORD(lParam));

        MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton2(message), MouseButtonEventArgs::Released, lButton, mButton, rButton, control, shift, x, y);
        pWindow->OnMouseButtonReleased(mouseButtonEventArgs);
      }
      break;
      case WM_MOUSEWHEEL:
      {
        // The distance the mouse wheel is rotated.
        // A positive value indicates the wheel was rotated to the right.
        // A negative value indicates the wheel was rotated to the left.
        float zDelta = ((int)(short)HIWORD(wParam)) / (float)WHEEL_DELTA;
        short keyStates = (short)LOWORD(wParam);

        bool lButton = (keyStates & MK_LBUTTON) != 0;
        bool rButton = (keyStates & MK_RBUTTON) != 0;
        bool mButton = (keyStates & MK_MBUTTON) != 0;
        bool shift = (keyStates & MK_SHIFT) != 0;
        bool control = (keyStates & MK_CONTROL) != 0;

        int x = ((int)(short)LOWORD(lParam));
        int y = ((int)(short)HIWORD(lParam));

        // Convert the screen coordinates to client coordinates.
        POINT clientToScreenPoint;
        clientToScreenPoint.x = x;
        clientToScreenPoint.y = y;
        ScreenToClient(hWnd, &clientToScreenPoint);

        MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
        pWindow->OnMouseWheel(mouseWheelEventArgs);
      }
      break;
      case WM_SIZE:
      {
        int width = ((int)(short)LOWORD(lParam));
        int height = ((int)(short)HIWORD(lParam));

        ResizeEventArgs resizeEventArgs(width, height);
        pWindow->OnResize(resizeEventArgs);
      }
      break;
      case WM_DESTROY:
      {
        // If a window is being destroyed, remove it from the 
        // window maps.
        pWindow->Unregister();
        //Application::RemoveWindow(hWnd);

        if (Application::NoWindowsLeft()) {
          // If there are no more windows, quit the application.
          PostQuitMessage(0);
        }
      }
      break;
      default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
      }
    }
    else {
      return DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return 0;
  }

  void Application::PlatformMainLoop(int& returnCode) {
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
      if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
      }
    }
    returnCode = static_cast<int>(msg.wParam);
  }

}