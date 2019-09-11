#include <Kame.h>

class ExampleLayer : public Kame::Layer {
  public:
  ExampleLayer()
    : Layer("Example") {}

  void OnUpdate() override {
    KM_INFO("ExampleLayer::Update");
  }

  void OnEvent(Kame::Event& event) override {
    KM_TRACE("{0}", event);
  }

};

class Sandbox : public Kame::Application {
  public:
  Sandbox() {
    PushLayer(new ExampleLayer());
    PushOverlay(new Kame::ImGuiLayer());
  }
  ~Sandbox() {}
};

Kame::Application* Kame::CreateApplication() {
  return new Sandbox();
}
