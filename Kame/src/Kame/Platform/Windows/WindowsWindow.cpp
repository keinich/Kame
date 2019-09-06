#include "kmpch.h"
#include "WindowsWindow.h"

namespace Kame {

  Window* Window::Create(const WindowProperties& props) {
    return new WindowsWindow(props);
  }

  static bool s_GLFWInitialized = false;

  WindowsWindow::WindowsWindow(const WindowProperties & props) {
    Init(props);
  }

  WindowsWindow::~WindowsWindow() {
    Shutdown();
  }

  void WindowsWindow::Init(const WindowProperties& props) {

    _Data.Title = props.Title;
    _Data.Width = props.Width;
    _Data.Height = props.Height;

    KM_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_GLFWInitialized) {
      int success = glfwInit();
      KM_CORE_ASSERT(success, "Could not initialize GLFW!");

      s_GLFWInitialized = true;
    }

    _Window = glfwCreateWindow((int)props.Width, (int)props.Height, _Data.Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_Window);
    glfwSetWindowUserPointer(_Window, &_Data);
    SetVSync(true);

  }

  void WindowsWindow::Shutdown() {
    glfwDestroyWindow(_Window);
  }

  void WindowsWindow::OnUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(_Window);
  }

  void WindowsWindow::SetVSync(bool enabled) {
    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

    _Data.VSync = enabled;
  }

  bool WindowsWindow::IsVSync() const {
    return _Data.VSync;
  }

}