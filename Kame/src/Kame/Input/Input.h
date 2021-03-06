#pragma once

#include <map>

#include "Kame/Input/KeyCodes.h"
#include "Kame/Events/Event.h"
#include "Kame/Events/EventArgs.h"
#include <Kame/Core/References.h>

namespace Kame {



  class KAME_API Input {

    friend class Application;
    friend class Window;

  public:

    //inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

    //inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
    //inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
    //inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
    //inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }

    static Reference<Event<KeyEventArgs>> KeyEvent(Kame::Key key);
    static Reference<Event<MouseButtonEventArgs>> MouseButtonEvent();
    static Reference<Event<MouseMotionEventArgs>> MouseMotionEvent();
    static Reference<Event<MouseWheelEventArgs>> MouseWheelEvent();
    static Reference<Event<KeyEventArgs>> CustomEvent(std::wstring name);
    static void MapKeyToCustomEvent(Kame::Key key, std::wstring name);

    virtual ~Input();

  protected:

    //virtual bool IsKeyPressedImpl(int keycode);

    //virtual bool IsMouseButtonPressedImpl(int button);
    //virtual float GetMouseXImpl();
    //virtual float GetMouseYImpl();
    //virtual std::pair<float, float> GetMousePositionImpl();

    bool OnKeyEvent(KeyEventArgs args);
    bool OnMappedKeyEvent(KeyEventArgs args);

    Input();
  private: // Methods


  private: // Fields

    std::map<Kame::Key, Reference<Event<KeyEventArgs>>> _KeyEvents;
    Reference<Event<MouseButtonEventArgs>> _MouseButtonEvent;
    Reference<Event<MouseMotionEventArgs>> _MouseMotionEvent;
    Reference<Event<MouseWheelEventArgs>> _MouseWheelEvent;
    std::map<std::wstring, Reference<Event<KeyEventArgs>>> _CustomEvents;
    std::map<Kame::Key, bool> _KeyStates;
    std::map<Kame::Key, std::wstring> _CustomEventNameByKey;

  };

}