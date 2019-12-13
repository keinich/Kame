#pragma once

#include "Kame/Events/Event_Cherno.h"

namespace Kame {

  class Camera;
  class Scene3D;

  struct ScreenRectangle {
    float Top = 0;
    float Left = 0;
    float Width = 1;
    float Height = 1;
  };

  class KAME_API Layer {
  public:
    Layer(const std::string& name = "Layer");
    ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnEvent(ChernoEvent& event) {}

    virtual Camera* GetActiveCamera() = 0;
    virtual Scene3D* GetScene() = 0;

    virtual ScreenRectangle GetScreenRectangle() { return _ScreenRectangle; }

    inline const std::string& GetName() const { return _DebugName; }

  protected:
    std::string _DebugName;
    ScreenRectangle _ScreenRectangle;
  };

}
