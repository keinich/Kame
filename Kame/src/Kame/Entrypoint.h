#pragma once

#ifdef KAME_PLATFORM_WINDOWS

extern Kame::Application* Kame::CreateApplication();

int main(int argc, char** argv) {

  Kame::Log::Init();
  KM_CORE_WARN("Initialized Log!");
  int a = 5;
  KM_INFO("Hello! Var={0}",a);

  auto app = Kame::CreateApplication();
  app->Run();
  delete app;
}

#endif
