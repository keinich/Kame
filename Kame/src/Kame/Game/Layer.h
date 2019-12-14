#pragma once

#include "Kame/Events/Event_Cherno.h"

#include <Kame/Core/References.h>
#include <Kame\Events\EventArgs.h>

namespace Kame {

  class Camera;
  class Scene3D;
  class RenderTarget;
  class Texture;

  struct ScreenRectangle {
    float Top = 0;
    float Left = 0;
    float Width = 1;
    float Height = 1;
  };

  class KAME_API Layer {
  public:
    Layer(UINT64 width, UINT height, const std::string& name = "Layer");
    ~Layer();


    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnEvent(ChernoEvent& event) {}

    virtual void OnResize(ResizeEventArgs& eventArgs);

    virtual Camera* GetActiveCamera() = 0;
    virtual Scene3D* GetScene() = 0;

    virtual ScreenRectangle GetScreenRectangle() { return _ScreenRectangle; }

    inline const std::string& GetName() const { return _DebugName; }

    inline RenderTarget* GetRenderTarget() { return _SceneRenderTarget.get(); }

  protected: // Functions

    virtual void Initialize();

    void CreateSceneTexture();
    void CreateSceneDepthTexture();

  protected: // Fields
    std::string _DebugName;
    ScreenRectangle _ScreenRectangle;

    Reference<Texture> _SceneTexture;
    Reference<Texture> _SceneDepthTexture;
    Reference<RenderTarget> _SceneRenderTarget;

    UINT64 _Width;
    UINT _Height;

  };

}
