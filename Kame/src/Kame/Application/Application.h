#pragma once

//#ifdef KAME_PLATFORM_WIN32
#define KAME_NATIVE_WINDOW HWND
namespace Kame {
  class Win32Window;
}
#define KAME_WINDOW Win32Window
//#endif

#include <map>

namespace Kame {

  class Game;
  class Window;
  class Input;

  using WindowPtr = std::shared_ptr<Window>;
  using WindowNameMap = std::map< std::wstring, WindowPtr >;

  class KAME_API Application {

  public:

    friend class Window;

    static Application& Get();
    static void Create();
    static void Destroy();

    void Initialize();

    void PlatformInitialize();

    inline uint64_t GetFrameCount() { return _FrameCount; }
    inline void IncrementFrameCount() { ++_FrameCount; }

    std::shared_ptr<Window> CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);
    void DestroyWindow(const std::wstring& windowName);
    void DestroyWindow(std::shared_ptr<Window> window);
    static std::shared_ptr<Window> GetWindowByName(const std::wstring& windowName);
    static std::shared_ptr<KAME_WINDOW> GetWindow(KAME_NATIVE_WINDOW nativeWindow);

    const static bool NoWindowsLeft();

    int Run(std::shared_ptr<Game> game);
    void PlatformMainLoop(int& returnCode);
    void Quit(int exitCode = 0);

    inline Input* GetInput() { return _Input; }

  protected: // Methods

    Application();
    virtual ~Application();

    static void RegisterWindow(std::shared_ptr<KAME_WINDOW> window);
    static void RemoveWindow(KAME_NATIVE_WINDOW nativeWindow);

  protected: // Fields

    uint64_t _FrameCount;
    std::shared_ptr<Game> _Game;
    Input* _Input;
  };

}