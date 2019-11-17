#include "kmpch.h"

#ifdef KAME_PLATFORM_DIRECTX12

#include "DX12Input.h"

#include <GLFW/glfw3.h>
#include <Kame/KameApplication.h>

namespace Kame {

  Input* Input::s_Instance = new DX12Input();

  bool DX12Input::IsKeyPressedImpl(int keycode) {
    return false;
  }

  bool DX12Input::IsMouseButtonPressedImpl(int button) {
    return false;
  }

  float DX12Input::GetMouseXImpl() {
    return 0.0;
  }

  float DX12Input::GetMouseYImpl() {
    return 0.0f;
  }

  std::pair<float, float> DX12Input::GetMousePositionImpl() {
    return { 0.0f,0.0f };
  }

}

#endif