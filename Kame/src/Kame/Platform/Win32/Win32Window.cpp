#include "kmpch.h"

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
    RegisterClassEx(&windowClass);

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

    //// Set GLFW callbacks
    //glfwSetWindowSizeCallback(
    //  _Window,
    //  [](GLFWwindow* window, int width, int height) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    //    data.Width = width;
    //    data.Height = height;

    //    WindowResizedEvent event(width, height);
    //    data.EventCallback(event);
    //  }
    //);

    //glfwSetWindowCloseCallback(
    //  _Window,
    //  [](GLFWwindow* window) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //    WindowCloseEvent event;
    //    data.EventCallback(event);
    //  }
    //);

    //glfwSetKeyCallback(
    //  _Window,
    //  [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //    switch (action) {
    //      case GLFW_PRESS:
    //      {
    //        KeyPressedEvent event(key, 0);
    //        data.EventCallback(event);
    //        break;
    //      }
    //      case GLFW_RELEASE:
    //      {
    //        KeyReleasedEvent event(key);
    //        data.EventCallback(event);
    //        break;
    //      }
    //      case GLFW_REPEAT:
    //      {
    //        KeyPressedEvent event(key, 1);
    //        data.EventCallback(event);
    //        break;
    //      }
    //    }
    //  }
    //);

    //glfwSetCharCallback(
    //  _Window,
    //  [](GLFWwindow* window, unsigned int keyCode) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //    KeyTypedEvent event(keyCode);
    //    data.EventCallback(event);
    //  }
    //);

    //glfwSetMouseButtonCallback(
    //  _Window,
    //  [](GLFWwindow* window, int button, int action, int mods) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //    switch (action) {
    //      case GLFW_PRESS:
    //      {
    //        MouseButtonPressedEvent event(button);
    //        data.EventCallback(event);
    //        break;
    //      }
    //      case GLFW_RELEASE:
    //      {
    //        MouseButtonReleasedEvent event(button);
    //        data.EventCallback(event);
    //        break;
    //      }
    //    }
    //  }
    //);

    //glfwSetScrollCallback(
    //  _Window,
    //  [](GLFWwindow* window, double xOffset, double yOffset) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
    //    MouseScrolledEvent event((float)xOffset, (float)yOffset);
    //    data.EventCallback(event);
    //  }
    //);

    //glfwSetCursorPosCallback(
    //  _Window,
    //  [](GLFWwindow* window, double x, double y) {
    //    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

    //    MouseMovedEvent event((float)x, (float)y);
    //    data.EventCallback(event);
    //  }
    //);
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

    switch (message) {
    case WM_CREATE:
    {
      LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
      SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;
    case WM_PAINT:
      return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
  }

}

#endif