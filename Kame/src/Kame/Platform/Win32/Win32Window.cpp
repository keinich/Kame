#include "kmpch.h"

//TODO Remove this
#include "Kame/Platform/DirectX12/Graphics/DxTutorial.h"

#ifdef KAME_PLATFORM_WIN32

#include "Win32Window.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Events/MouseEvent.h"
#include "Kame/Events/KeyEvent.h"

#include "Kame/Application.h"

namespace Kame {

  static bool s_GLFWInitialized = false;

  static void GLFWErrorCallback(int error, const char* description) {
    KM_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
  }

  Window* Window::Create(const WindowProperties& props) {
    return new Win32Window(props);
  }

  Win32Window::Win32Window(const WindowProperties& props) {
    Init(props);
  }

  Win32Window::~Win32Window() {
    Shutdown();
  }

  void Win32Window::Init(const WindowProperties& props) {

    _Data.Title = props.Title;
    _Data.Width = props.Width;
    _Data.Height = props.Height;

    KM_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = Win32Window::WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";

    static HRESULT hr = ::RegisterClassEx(&windowClass);
    assert(SUCCEEDED(hr));


    RECT windowRect = { 0,0,1280,720 }; //TODO who sets the window dimensions first?
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND w = CreateWindowW(
      windowClass.lpszClassName,
      L"Test",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      windowRect.right - windowRect.left,
      windowRect.bottom - windowRect.top,
      nullptr,
      nullptr,
      hInstance,
      &Application::Get() //TODO what is this param?
    );

    _Window = &w;

  }

  void Win32Window::Shutdown() {}

  void Win32Window::OnUpdate() {}

  void Win32Window::SetVSync(bool enabled) {
    _Data.VSync = enabled;
  }

  bool Win32Window::IsVSync() const {
    return _Data.VSync;
  }

  LRESULT Win32Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    if (DxTutorial::_Instance->_IsInitialized) {
      switch (message) {
      case WM_CREATE:
      {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
      }
      case WM_SIZE:
      {
        RECT clientRect = {};
        ::GetClientRect(hWnd, &clientRect);

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        DxTutorial::_Instance->Resize(width, height);
      }
      break;
      return 0;
      case WM_PAINT:
        //TODO make this better!!!!
        DxTutorial::_Instance->Update();
        DxTutorial::_Instance->Render();
        break;
        return 0;
      }

      return DefWindowProc(hWnd, message, wParam, lParam);
    }

    else {
      return ::DefWindowProcW(hWnd, message, wParam, lParam);

    }
  }

}

#endif