#pragma once

#include "Window.h"
#include "Kame/Layer.h"
#include "Kame/LayerStack.h"
#include "Core.h"
#include "Events/Event.h"

namespace Kame {

  class KAME_API Application {
  public:
    Application();
    virtual ~Application();

    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    inline static Application& Get() { return *s_Instance; }
    inline Window& GetWindow() { return *_Window; }

  private:
    std::unique_ptr<Window> _Window;
    bool _Running = true;
    LayerStack _LayerStack;
  
    static Application*  s_Instance;
  };

  // to be defined in the client
  Application* CreateApplication();

}

