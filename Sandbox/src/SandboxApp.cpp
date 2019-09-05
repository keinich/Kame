#include <Kame.h>

class Sandbox : public Kame::Application {
public:
  Sandbox() {}
  ~Sandbox() {}
};

Kame::Application* Kame::CreateApplication() {
  return new Sandbox();
}
