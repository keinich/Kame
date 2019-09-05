#pragma once

#include "Core.h"

namespace Kame {

  class KAME_API Application {
  public:
    Application();
    virtual ~Application();

    void Run();
  };

  // to be defined in the client
  Application* CreateApplication();

}

