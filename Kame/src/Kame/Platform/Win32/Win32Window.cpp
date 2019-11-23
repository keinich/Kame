#include "kmpch.h"
#include "Win32Window.h"
#include "Kame/Application.h"


namespace Kame {

  Win32Window::Win32Window(const std::wstring& name, int width, int height, bool vSync) :
    Window(name, width, height, vSync) {}

}