#include "kmpch.h"
#include "Application.h"
//#include "Kame/Platform/DirectX12/Graphics/Display.h"
#include "Kame/Window.h"

#include "Kame/Platform/DirectX12/Graphics/DX12Core.h"
#include "Kame/Platform/DirectX12/Graphics/Game.h"

namespace Kame {

  static Application* _Instance = nullptr;
  static WindowNameMap s_WindowByName;



  using WindowMap = std::map< KAME_NATIVE_WINDOW, WindowPtr >;
  static WindowMap s_Windows;

  //uint64_t Application::ms_FrameCount = 0;

  Application& Application::Get() {
    assert(_Instance);
    return *_Instance;
  }

  void Application::Create() {
    assert(!_Instance, "Application is already initialized");
    _Instance = new Application();
    _Instance->Initialize();
  }

  void Application::Destroy() {
    if (_Instance) {
      assert(
        s_Windows.empty() && s_WindowByName.empty() &&
        "All windows should be destroyed before destroying the application instance."
      );

      delete _Instance;
      _Instance = nullptr;
    }
  }

  Application::Application() {
    _FrameCount = 0;
  }

  Application::~Application() {
    DX12Core::Get().Flush();
  }

  void Application::Initialize() { // Windows Application Init


    PlatformInitialize();

    DX12Core::Get().Initialize();

  }

  struct MakeWindow : public Window {
    MakeWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
      : Window(windowName, clientWidth, clientHeight, vSync) {}
  };

  std::shared_ptr<Window> Application::CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync) {
    // First check if a window with the given name already exists.
    WindowNameMap::iterator windowIter = s_WindowByName.find(windowName);
    if (windowIter != s_WindowByName.end()) {
      return windowIter->second;
    }

    WindowPtr pWindow = std::make_shared<MakeWindow>(windowName, clientWidth, clientHeight, vSync);
    pWindow->GetDisplay().Initialize();

    RegisterWindow(pWindow);

    return pWindow;
  }

  void Application::DestroyWindow(std::shared_ptr<Window> window) {
    if (window) window->Destroy();
  }

  void Application::DestroyWindow(const std::wstring& windowName) {
    WindowPtr pWindow = GetWindowByName(windowName);
    if (pWindow) {
      DestroyWindow(pWindow);
    }
  }

  std::shared_ptr<Window> Application::GetWindowByName(const std::wstring& windowName) {
    std::shared_ptr<Window> window;
    WindowNameMap::iterator iter = s_WindowByName.find(windowName);
    if (iter != s_WindowByName.end()) {
      window = iter->second;
    }
    return window;
  }

  std::shared_ptr<Window> Application::GetWindow(KAME_NATIVE_WINDOW nativeWindow) {
    WindowMap::iterator iter = s_Windows.find(nativeWindow);
    if (iter != s_Windows.end()) {
      return iter->second;
    }
    else {
      return nullptr;
    }
  }

  int Application::Run(std::shared_ptr<Game> game) {
    if (!game->Initialize()) return 1;
    if (!game->LoadContent()) return 2;

    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
      if (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
      }
    }

    // Flush any commands in the commands queues before quiting.
    DX12Core::Get().Flush();

    game->UnloadContent();
    game->Destroy();

    return static_cast<int>(msg.wParam);
  }

  void Application::Quit(int exitCode) {
    PostQuitMessage(exitCode);
  }

  // Remove a window from our window lists.
  void Application::RemoveWindow(KAME_NATIVE_WINDOW nativeWindow) {
    WindowMap::iterator windowIter = s_Windows.find(nativeWindow);
    if (windowIter != s_Windows.end()) {
      WindowPtr pWindow = windowIter->second;
      s_WindowByName.erase(pWindow->GetWindowName());
      s_Windows.erase(windowIter);
    }
  }

  const bool Application::NoWindowsLeft() {
    return s_Windows.empty();
  }

  void Application::RegisterWindow(std::shared_ptr<Window> window) {
    s_Windows.insert(WindowMap::value_type(window->GetWindowHandle(), window));
    s_WindowByName.insert(WindowNameMap::value_type(window->GetName(), window));
  }

  // WIN32:

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

    WindowPtr pWindow = Application::GetWindow(hWnd);
    
     
    

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
        KeyCode::Key key = (KeyCode::Key)wParam;
        unsigned int scanCode = (lParam & 0x00FF0000) >> 16;
        KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Pressed, shift, control, alt);
        pWindow->OnKeyPressed(keyEventArgs);
      }
      break;
      case WM_SYSKEYUP:
      case WM_KEYUP:
      {
        bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        KeyCode::Key key = (KeyCode::Key)wParam;
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
        Application::RemoveWindow(hWnd);

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

}