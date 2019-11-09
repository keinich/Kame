#pragma once

#include "Window.h"
#include "Kame/Layer.h"
#include "Kame/LayerStack.h"
#include "Core.h"
#include "Events/Event.h"
#include "Events/KeyEvent.h"

namespace Kame {

  class KAME_API Application {

    friend class Window;

  public:
    Application();
    virtual ~Application();

    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    inline static Application& Get() { return *s_Instance; }
    inline Window& GetWindow() { return *_Window; }

    void Update();
    void Render();

    inline const uint64_t const GetFrameCount() { return _FrameCount; }

  protected: // Fields

    uint64_t _FrameCount;

  private: //Functions
    void PlatformMainLoop();
    void CountFps();

    bool OnKeyPressedEvent(KeyPressedEvent& e);

  private:


    std::unique_ptr<Window> _Window;
    bool _Running = true;
    LayerStack _LayerStack;

    static Application* s_Instance;
  };

  // to be defined in the client
  Application* CreateApplication();

}

