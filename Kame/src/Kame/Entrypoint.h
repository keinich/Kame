#pragma once

#ifdef KAME_PLATFORM_WINDOWS

extern Kame::Application* Kame::CreateApplication();

int main(int argc, char** argv) {
  auto app = Kame::CreateApplication();
  app->Run();
  delete app;
}

#endif
