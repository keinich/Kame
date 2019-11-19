#include "kmpch.h"
#include "Window.h"

namespace Kame {

  Window::Window(HWND hWnd, const std::wstring& name, int width, int height, bool vSync) :
    _Display(hWnd, name, width, height, vSync) {}

  Window::~Window() {}

}
