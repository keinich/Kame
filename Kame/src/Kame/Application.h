#pragma once

namespace Kame {

  class Game;
  class Window;

  class KAME_API Application {

  public:

    static Application& Get();
    static void Create();
    static void Destroy();

    void Initialize();

    inline uint64_t GetFrameCount() { return _FrameCount; }
    inline void IncrementFrameCount() { ++_FrameCount; }

    std::shared_ptr<Window> CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);
    void DestroyWindow(const std::wstring& windowName);
    void DestroyWindow(std::shared_ptr<Window> window);
    std::shared_ptr<Window> GetWindowByName(const std::wstring& windowName);

    int Run(std::shared_ptr<Game> game);
    void Quit(int exitCode = 0);

  protected: // Methods
    Application();
    virtual ~Application();

  protected: // Fields

    uint64_t _FrameCount;
  };

}