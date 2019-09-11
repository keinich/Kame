#include "kmpch.h"
#include "GLFWWindow.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Events/MouseEvent.h"
#include "Kame/Events/KeyEvent.h"

#include <glad/glad.h>

namespace Kame {

  static bool s_GLFWInitialized = false;

  static void GLFWErrorCallback(int error, const char* description) {
    KM_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
  }

  Window* Window::Create(const WindowProperties& props) {
    return new GLFWWindow(props);
  }

  GLFWWindow::GLFWWindow(const WindowProperties & props) {
    Init(props);
  }

  GLFWWindow::~GLFWWindow() {
    Shutdown();
  }

  void GLFWWindow::Init(const WindowProperties& props) {

    _Data.Title = props.Title;
    _Data.Width = props.Width;
    _Data.Height = props.Height;

    KM_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_GLFWInitialized) {
      int success = glfwInit();
      KM_CORE_ASSERT(success, "Could not initialize GLFW!");
      glfwSetErrorCallback(GLFWErrorCallback);
      s_GLFWInitialized = true;
    }

    _Window = glfwCreateWindow((int)props.Width, (int)props.Height, _Data.Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(_Window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    KM_CORE_ASSERT(status, "Failed to initialize Glad!");
    glfwSetWindowUserPointer(_Window, &_Data);
    SetVSync(true);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(
      _Window,
      [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizedEvent event(width, height);
        data.EventCallback(event);
      }
    );

    glfwSetWindowCloseCallback(
      _Window,
      [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        WindowCloseEvent event;
        data.EventCallback(event);
      }
    );

    glfwSetKeyCallback(
      _Window,
      [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
          case GLFW_PRESS:
          {
            KeyPressedEvent event(key, 0);
            data.EventCallback(event);
            break;
          }
          case GLFW_RELEASE:
          {
            KeyReleasedEvent event(key);
            data.EventCallback(event);
            break;
          }
          case GLFW_REPEAT:
          {
            KeyPressedEvent event(key, 1);
            data.EventCallback(event);
            break;
          }
        }
      }
    );

    glfwSetCharCallback(
      _Window,
      [](GLFWwindow* window, unsigned int keyCode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keyCode);
        data.EventCallback(event);
      }
    );

    glfwSetMouseButtonCallback(
      _Window,
      [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action) {
          case GLFW_PRESS:
          {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
          }
          case GLFW_RELEASE:
          {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            break;
          }
        }
      }
    );

    glfwSetScrollCallback(
      _Window,
      [](GLFWwindow* window, double xOffset, double yOffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.EventCallback(event);
      }
    );

    glfwSetCursorPosCallback(
      _Window,
      [](GLFWwindow* window, double x, double y) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        MouseMovedEvent event((float)x, (float)y);
        data.EventCallback(event);
      }
    );
  }

  void GLFWWindow::Shutdown() {
    glfwDestroyWindow(_Window);
  }

  void GLFWWindow::OnUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(_Window);
  }

  void GLFWWindow::SetVSync(bool enabled) {
    if (enabled)
      glfwSwapInterval(1);
    else
      glfwSwapInterval(0);

    _Data.VSync = enabled;
  }

  bool GLFWWindow::IsVSync() const {
    return _Data.VSync;
  }

}