#include "Application.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Log.h"

namespace Kame {

  Application::Application() {
  }


  Application::~Application() {
  }

  void Application::Run() {
    WindowResizeEvent e(1280, 720);
    KM_WARN(e);
    while (true);
  }

}
