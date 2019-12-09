#include "kmpch.h"
#include "Input.h"
#include "Kame/Application/Application.h"

namespace Kame {

  Reference<Event<KeyEventArgs>> Input::KeyEvent(Kame::Key key) {
    //TODO could init one event for each key to save performance...
    auto input = Application::Get().GetInput();
    if (!input)
      return Reference<Event<KeyEventArgs>>();
    std::map<Kame::Key, Reference<Event<KeyEventArgs>>>::iterator x = input->_KeyEvents.find(key);
    if (x == input->_KeyEvents.end()) {
      Reference<Event<KeyEventArgs>> e = CreateReference< Event<KeyEventArgs>>();
      //e->AddHandler(BIND_FUNCTION_I(input, Input::OnKeyEvent)); //TODO das hier ist sowieso fragwürdig,
      // Wenn aus irgendeinem Grund zb das Release-Event nicht gefeuert wird, bleibt die Taste gefrückt
      // zb im Debugging kann das passieren
      input->_KeyEvents.insert(
        std::map<Kame::Key, Reference<Event<KeyEventArgs>>>::value_type(key, e)
      );
      return e;
    }
    else {
      return x->second;
    }
  }

  Reference<Event<MouseButtonEventArgs>> Input::MouseButtonEvent() {
    auto input = Application::Get().GetInput();
    if (!input)
      return Reference<Event<MouseButtonEventArgs>>();
    if (!input->_MouseButtonEvent.get()) {
      input->_MouseButtonEvent = CreateReference< Event<MouseButtonEventArgs>>();
    }
    return input->_MouseButtonEvent;
  }

  Reference<Event<MouseMotionEventArgs>> Input::MouseMotionEvent() {
    auto input = Application::Get().GetInput();
    if (!input)
      return Reference<Event<MouseMotionEventArgs>>();
    if (!input->_MouseMotionEvent.get()) {
      input->_MouseMotionEvent = CreateReference< Event<MouseMotionEventArgs>>();
    }
    return input->_MouseMotionEvent;
  }

  Reference<Event<MouseWheelEventArgs>> Input::MouseWheelEvent() {
    auto input = Application::Get().GetInput();
    if (!input)
      return Reference<Event<MouseWheelEventArgs>>();
    if (!input->_MouseWheelEvent.get()) {
      input->_MouseWheelEvent = CreateReference< Event<MouseWheelEventArgs>>();
    }
    return input->_MouseWheelEvent;
  }

  Reference<Event<KeyEventArgs>> Input::CustomEvent(std::wstring name) {
    //TODO could init one event for each key to save performance...
    auto input = Application::Get().GetInput();
    std::map<std::wstring, Reference<Event<KeyEventArgs>>>::iterator x = input->_CustomEvents.find(name);
    if (x == input->_CustomEvents.end()) {
      Reference<Event<KeyEventArgs>> e = CreateReference< Event<KeyEventArgs>>();
      //e->AddHandler(BIND_FUNCTION_I(input, Input::OnKeyEvent)); //TODO das hier ist sowieso fragwürdig,
      // Wenn aus irgendeinem Grund zb das Release-Event nicht gefeuert wird, bleibt die Taste gefrückt
      // zb im Debugging kann das passieren
      input->_CustomEvents.insert(
        std::map<std::wstring, Reference<Event<KeyEventArgs>>>::value_type(name, e)
      );
      return e;
    }
    else {
      return x->second;
    }
  }

  void Input::MapKeyToCustomEvent(Kame::Key key, std::wstring name) {
    auto input = Application::Get().GetInput();
    input->KeyEvent(key)->AddHandler("Input::OnMappedKeyEvent", BIND_FUNCTION_I(input, Input::OnMappedKeyEvent));
    input->_CustomEventNameByKey[key] = name;
  }

  Input::~Input() {
    _KeyEvents.clear();
    _KeyEvents.clear();
    _CustomEventNameByKey.clear();
    _CustomEvents.clear();
  }

  bool Input::OnKeyEvent(KeyEventArgs args) {
    auto input = Application::Get().GetInput();
    Kame::Key key = args.Key;
    bool pressed = args.State;
    std::map<Kame::Key, bool>::iterator x = input->_KeyStates.find(key);
    if (x == input->_KeyStates.end()) {
      input->_KeyStates.insert(
        std::map<Kame::Key, bool>::value_type(key, pressed)
      );
    }
    else {
      input->_KeyStates[key] = pressed;
    }
    return true;
  }

  bool Input::OnMappedKeyEvent(KeyEventArgs args) {
    auto input = Application::Get().GetInput();
    Kame::Key key = args.Key;
    input->CustomEvent(input->_CustomEventNameByKey[key])->Raise(args);
    return true;
  }

  Input::Input() {}

}

