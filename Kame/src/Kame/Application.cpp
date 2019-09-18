#include "kmpch.h"
#include "Application.h"

#include "Kame/Events/ApplicationEvent.h"
#include "Kame/Log.h"

#ifdef KAME_PLATFORM_OPENGL
#include <glad/glad.h>
#else
#include "Kame/Platform/DirectX12/Graphics/DxTutorial.h"
#endif

#include "Input.h"

namespace Kame {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

  Application* Application::s_Instance = nullptr;

  Application::Application() {

    KM_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

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
    DxTutorial::_Instance->Update();
  }

  void Application::Render() {
    DxTutorial::_Instance->Render();
  }

  void Application::Run() {
    while (_Running) {

      _Window = std::unique_ptr<Window>(Window::Create());
      auto test = _Window->GetNativeWindow();
      HWND test2 = (HWND)test;

#ifdef KAME_PLATFORM_OPENGL
      glClearColor(1, 0, 1, 1);
      glClear(GL_COLOR_BUFFER_BIT);
#else
      DxTutorial* dxTest = DxTutorial::_Instance;
      dxTest->Init();
#endif      

      _Window->Show();

      PlatformRun(std::bind(&Application::Update, this), std::bind(&Application::Render, this));

      for (Layer* layer : _LayerStack)
        layer->OnUpdate();

      //auto[x, y] = Input::GetMousePosition();
      //KM_CORE_TRACE("{0}, {1}", x, y);

      _Window->OnUpdate();
    }
  }

  void Application::OnEvent(Event & e) {

    //KM_CORE_INFO("{0}", e);

    for (auto it = _LayerStack.end(); it != _LayerStack.begin();) {
      (*--it)->OnEvent(e);
      if (e.IsHandled())
        break;
    }

  }

}
