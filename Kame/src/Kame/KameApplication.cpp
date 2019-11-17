#include "kmpch.h"
#include "KameApplication.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Events/KeyEvent.h"
#include "Kame/Log.h"
#include "Kame/KeyCodes.h"

#ifdef KAME_PLATFORM_OPENGL
#include <glad/glad.h>
#else
//#include "Kame/Platform/DirectX12/Graphics/DX12Core.h"
#include "Kame/Platform/DirectX12/3dgep/Application.h"
#endif

#include "Input.h"

namespace Kame {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() {

    KM_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    _FrameCount = 0;

    //_Window = std::unique_ptr<Window>(Window::Create());
    //_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    unsigned int id;
    //glGenVertexArrays(1, &id);
  }

  Application::~Application() {}

  void Application::PushLayer(Layer* layer) {
    _LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* layer) {
    _LayerStack.PushOverlay(layer);
    layer->OnAttach();
  }

  void Application::Update() {

    ++_FrameCount;

    for (Layer* layer : _LayerStack)
      layer->OnUpdate();

    _Window->OnUpdate();

    // FPS Counter
    CountFps();
  }

  void Application::CountFps() {
    static uint64_t frameCounter = 0;
    static double elapsedSeconds = 0.0;
    static std::chrono::high_resolution_clock clock;
    static auto t0 = clock.now();

    frameCounter++;
    auto t1 = clock.now();
    auto deltaTime = t1 - t0;
    t0 = t1;

    elapsedSeconds += deltaTime.count() * 1e-9;
    if (elapsedSeconds > 1.0) {
      char buffer[500];
      auto fps = frameCounter / elapsedSeconds;
      sprintf_s(buffer, 500, "FPS: %f\n", fps);
      KM_INFO(buffer);

      frameCounter = 0;
      elapsedSeconds = 0.0;
    }
  }

  bool Application::OnKeyPressedEvent(KeyPressedEvent& e) {
    switch (e.GetKeyCode()) 
      case KAME_KEY_F:
    {
      DX12Core::Get()->SwitchFullscreen();
      return true;
    }
    return false;
  }

  void Application::Render() {
    DX12Core::Get()->Render();
  }

  void Application::Run() {
    //while (_Running) {

      _Window = std::unique_ptr<Window>(Window::Create());
      _Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

#ifdef KAME_PLATFORM_OPENGL
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
#else
      //DX12Core* dxTest = DX12Core::Get();
      DX12Core::Get()->Init();
#endif      

      _Window->Show();

      PlatformMainLoop();

      DX12Core::Get()->ShutDown();

      //delete DX12Core::Get();
    //}
  }

  void Application::OnEvent(Event& e) {

    KM_CORE_INFO("{0}", e);    

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<KeyPressedEvent>(KM_BIND_EVENT_FN(Application::OnKeyPressedEvent));

    for (auto it = _LayerStack.end(); it != _LayerStack.begin();) {
      (*--it)->OnEvent(e);
      if (e.IsHandled())
        break;
    }

  }

}
