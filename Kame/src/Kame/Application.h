#pragma once

//#ifdef KAME_PLATFORM_WIN32
#define KAME_NATIVE_WINDOW HWND
//#endif

#include <map>

namespace Kame {

  class Game;
  class Window;

  using WindowPtr = std::shared_ptr<Window>;
  using WindowNameMap = std::map< std::wstring, WindowPtr >;

  class KAME_API Application {

  public:

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
    static std::shared_ptr<Window> GetWindow(KAME_NATIVE_WINDOW nativeWindow);

    static void RegisterWindow(std::shared_ptr<Window> window);
    static void RemoveWindow(KAME_NATIVE_WINDOW nativeWindow);

    const static bool NoWindowsLeft();

    int Run(std::shared_ptr<Game> game);
    void Quit(int exitCode = 0);

  protected: // Methods

    Application();
    virtual ~Application();

  protected: // Fields

    uint64_t _FrameCount;
  };

}