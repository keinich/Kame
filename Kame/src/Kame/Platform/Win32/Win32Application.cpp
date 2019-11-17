#include "kmpch.h"

#include "Kame/KameApplication.h"

namespace Kame {

  void Application::PlatformMainLoop() {
       
    MSG msg = {};
    while (msg.message != WM_QUIT) {
      if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    } 
  }

}