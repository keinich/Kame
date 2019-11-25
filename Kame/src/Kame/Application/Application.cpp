#include "kmpch.h"
#include "Application.h"
//#include "Kame/Platform/DirectX12/Graphics/Display.h"
#include <Kame/Application/Window.h>
#include <Kame/Platform/Win32/Win32Window.h>
         
#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Platform/DirectX12/Graphics/Game.h>

#include <Kame/Input/Input.h>
//#include "Kame/Events/Event.h"

namespace Kame {

  static Application* _Instance = nullptr;
  static WindowNameMap s_WindowByName;



  //using WindowMap = std::map< KAME_NATIVE_WINDOW, WindowPtr >;
  using WindowMap = std::map< KAME_NATIVE_WINDOW, std::shared_ptr<Win32Window> >;
  static WindowMap s_Windows;

  //uint64_t Application::ms_FrameCount = 0;

  Application& Application::Get() {
    assert(_Instance);
    return *_Instance;
  }

  void Application::Create() {
    assert(!_Instance, "Application is already initialized");
    _Instance = new Application();
    _Instance->Initialize();
  }

  void Application::Destroy() {
    if (_Instance) {
      assert(
        s_Windows.empty() && s_WindowByName.empty() &&
        "All windows should be destroyed before destroying the application instance."
      );

      delete _Instance;
      _Instance = nullptr;
    }
  }

  Application::Application() {
    _FrameCount = 0;
    _Input = new Input();
  }

  Application::~Application() {
    DX12Core::Get().Flush();
    delete _Input;
    _Input = nullptr;
  }

  void Application::Initialize() {

    PlatformInitialize();
    DX12Core::Get().Initialize();

  }

  struct MakeWindow : public KAME_WINDOW {
    MakeWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync)
      : KAME_WINDOW(windowName, clientWidth, clientHeight, vSync) {}
  };

  std::shared_ptr<Window> Application::CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync) {
    // First check if a window with the given name already exists.
    WindowNameMap::iterator windowIter = s_WindowByName.find(windowName);
    if (windowIter != s_WindowByName.end()) {
      return windowIter->second;
    }

    std::shared_ptr<KAME_WINDOW> pWindow = std::make_shared<MakeWindow>(windowName, clientWidth, clientHeight, vSync);
    pWindow->GetDisplay().Initialize();

    RegisterWindow(pWindow);

    return pWindow;
  }

  void Application::DestroyWindow(std::shared_ptr<Window> window) {
    if (window) window->Destroy();
  }

  void Application::DestroyWindow(const std::wstring& windowName) {
    WindowPtr pWindow = GetWindowByName(windowName);
    if (pWindow) {
      DestroyWindow(pWindow);
    }
  }

  std::shared_ptr<Window> Application::GetWindowByName(const std::wstring& windowName) {
    std::shared_ptr<Window> window;
    WindowNameMap::iterator iter = s_WindowByName.find(windowName);
    if (iter != s_WindowByName.end()) {
      window = iter->second;
    }
    return window;
  }

  std::shared_ptr<KAME_WINDOW> Application::GetWindow(KAME_NATIVE_WINDOW nativeWindow) {
    WindowMap::iterator iter = s_Windows.find(nativeWindow);
    if (iter != s_Windows.end()) {
      return iter->second;
    }
    else {
      return nullptr;
    }
  }

  int Application::Run(std::shared_ptr<Game> game) {

    _Game = game;

    if (!game->Initialize()) return 1;
    if (!game->LoadContent()) return 2;

    int returnCode;

    PlatformMainLoop(returnCode);

    // Flush any commands in the commands queues before quiting.
    DX12Core::Get().Flush();

    game->UnloadContent();
    game->Destroy();

    return returnCode;
  }

  void Application::Quit(int exitCode) {
    PostQuitMessage(exitCode);
  }

  // Remove a window from our window lists.
  void Application::RemoveWindow(KAME_NATIVE_WINDOW nativeWindow) {
    WindowMap::iterator windowIter = s_Windows.find(nativeWindow);
    if (windowIter != s_Windows.end()) {
      WindowPtr pWindow = windowIter->second;
      s_WindowByName.erase(pWindow->GetWindowName());
      s_Windows.erase(windowIter);
    }
  }

  const bool Application::NoWindowsLeft() {
    return s_Windows.empty();
  }

  void Application::RegisterWindow(std::shared_ptr<KAME_WINDOW> window) {
    //std::shared_ptr<Win32Window> test; /*= std::make_shared<Win32Window>(L"Test", 100, 100, true);*/
    //s_Windows.insert(WindowMap::value_type(test->GetWindowHandle(), test));

    s_Windows.insert(WindowMap::value_type(window->GetNativeWindow(), window));
    s_WindowByName.insert(WindowNameMap::value_type(window->GetName(), window));
  }

  // WIN32:

}