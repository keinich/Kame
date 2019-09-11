#pragma once

#include "Kame/Layer.h"

#include "Kame/Events/KeyEvent.h"
#include "Kame/Events/MouseEvent.h"
#include "Kame/Events/ApplicationEvent.h"

namespace Kame {

  class KAME_API ImGuiLayer : public Layer {

  public:
    ImGuiLayer();
    ~ImGuiLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(class Event& event) override;

  private:
    bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
    bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
    bool OnMouseMovedEvent(MouseMovedEvent& e);
    bool OnMouseScrolledEvent(MouseScrolledEvent& e);
    bool OnKeyPressedEvent(KeyPressedEvent& e);
    bool OnKeyReleasedEvent(KeyReleasedEvent& e);
    bool OnKeyTypedEvent(KeyTypedEvent& e);
    bool OnWindowResizedEvent(WindowResizedEvent& e);

  private:
    float _Time = 0.0f;

  };

}