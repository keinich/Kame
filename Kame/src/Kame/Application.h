#pragma once

namespace Kame {

  class Game;
  class Display;

  class KAME_API Application {

  public:

    static Application& Get();
    static void Create(HINSTANCE hInst);
    static void Destroy();

    void Initialize();

    inline uint64_t GetFrameCount() { return _FrameCount; }
    inline void IncrementFrameCount() { ++_FrameCount; }

    std::shared_ptr<Display> CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);
    void DestroyWindow(const std::wstring& windowName);
    void DestroyWindow(std::shared_ptr<Display> window);
    std::shared_ptr<Display> GetWindowByName(const std::wstring& windowName);

    int Run(std::shared_ptr<Game> game);
    void Quit(int exitCode = 0);

  protected: // Methods
    Application(HINSTANCE hInst);
    virtual ~Application();

  protected: // Fields    

    HINSTANCE _hInstance;
    uint64_t _FrameCount;
  };

}