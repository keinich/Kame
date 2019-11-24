//#pragma once
//
//#include "Display.h"
//#include "Kame/Layer.h"
//#include "Kame/LayerStack.h"
//#include "Core.h"
//#include "Events/Event.h"
//#include "Events/KeyEvent.h"
//
//namespace Kame {
//
//  class KAME_API DX12Core {
//
//    friend class Display;
//
//  public:
//    DX12Core();
//    virtual ~DX12Core();
//
//    void Run();
//
//    void OnEvent(Event& e);
//
//    void PushLayer(Layer* layer);
//    void PushOverlay(Layer* layer);
//
//    inline static DX12Core& Get() { return *s_Instance; }
//    inline Display& GetWindow() { return *_Window; }
//
//    void Update();
//    void Render();
//
//    inline const uint64_t const GetFrameCount() { return _FrameCount; }
//
//  protected: // Fields
//
//    uint64_t _FrameCount;
//
//  private: //Functions
//    void PlatformMainLoop();
//    void CountFps();
//
//    bool OnKeyPressedEvent(KeyPressedEvent& e);
//
//  private:
//
//
//    std::unique_ptr<Display> _Window;
//    bool _Running = true;
//    LayerStack _LayerStack;
//
//    static DX12Core* s_Instance;
//  };
//
//  // to be defined in the client
//  DX12Core* CreateApplication();
//
//}
//
