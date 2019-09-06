#include "kmpch.h"
#include "Application.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Log.h"

#include <GLFW/glfw3.h>

namespace Kame {

  Application::Application() {
    _Window = std::unique_ptr<Window>(Window::Create());
  }

  Application::~Application() {
  }

  void Application::Run() {
    while (_Running) {
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
      _Window->OnUpdate();
    }
  }

}
