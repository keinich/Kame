#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

namespace Kame {

  class KAME_API Application {
  public:
    Application();
    virtual ~Application();

    void Run();

    private:
    std::unique_ptr<Window> _Window;
    bool _Running = true;
  };

  // to be defined in the client
  Application* CreateApplication();

}

