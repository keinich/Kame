#include "kmpch.h"

#ifdef KAME_PLATFORM_GLFW

#include "GLFWInput.h"

#include <GLFW/glfw3.h>
#include <Kame/Application.h>

namespace Kame {

  Input* Input::s_Instance = new GLFWInput();

  bool GLFWInput::IsKeyPressedImpl(int keycode) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    int state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
  }

  bool GLFWInput::IsMouseButtonPressedImpl(int button) {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    int state = glfwGetMouseButton(window, button);
    return state = GLFW_PRESS;
  }

  float GLFWInput::GetMouseXImpl() {
    auto[x, y] = GetMousePositionImpl();
    return x;
  }

  float GLFWInput::GetMouseYImpl() {
    auto[x, y] = GetMousePositionImpl();
    return y;
  }

  std::pair<float, float> GLFWInput::GetMousePositionImpl() {
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return { (float)xpos,(float)ypos };
  }

}

#endif