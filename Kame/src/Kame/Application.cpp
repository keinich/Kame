#include "kmpch.h"
#include "Application.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Log.h"

#include <GLFW/glfw3.h>

namespace Kame {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

  Application::Application() {
    _Window = std::unique_ptr<Window>(Window::Create());
    _Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
  }

  Application::~Application() {}

  void Application::PushLayer(Layer* layer) {
    _LayerStack.PushLayer(layer);
  }

  void Application::PushOverlay(Layer* layer) {
    _LayerStack.PushOverlay(layer);
  }

  void Application::Run() {
    while (_Running) {
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      for (Layer* layer : _LayerStack)
        layer->OnUpdate();

      _Window->OnUpdate();
    }
  }

  void Application::OnEvent(Event & e) {

    KM_CORE_INFO("{0}", e);

    for (auto it = _LayerStack.end(); it != _LayerStack.begin();) {
      (*--it)->OnEvent(e);
      if (e.IsHandled())
        break;
    }

  }

}
