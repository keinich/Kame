#include "kmpch.h"

#include "Kame/Application.h"

namespace Kame {

  void Application::PlatformRun(std::function<void()> onUpdate, std::function<void()> onRender) {
       
    MSG msg = {};
    while (msg.message != WM_QUIT) {
      if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    }
  }

}