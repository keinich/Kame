#pragma once

#include "Kame/Events/Event_Cherno.h"

#include <Kame/Core/References.h>
#include <Kame\Events\EventArgs.h>

#include <Kame/Graphics/2D/ScreenRectangle.h>
#include <Kame/Gui/GuiScene.h>

namespace Kame {

  class Camera;
  class Scene3D;
  class RenderTarget;
  class Texture;
  class Game;

  class KAME_API Layer {
  public:
    Layer(
      Game* parent,
      float relativeLeft, float relativeTop, float relativeWidth, float relativeHeight,
      const std::string& name
    );

    ~Layer();


    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(UpdateEventArgs& e) {
      _GuiScene->Update(e);
    }
    virtual void OnEvent(ChernoEvent& event) {}

    virtual void OnResize(ResizeEventArgs& eventArgs);

    virtual Camera* GetActiveCamera() = 0;
    virtual Scene3D* GetScene() = 0;

    inline GuiScene* GetGuiScene() { return _GuiScene.get(); }

    virtual ScreenRectangle GetScreenRectangle() { return _ScreenRectangle; }

    inline const std::string& GetName() const { return _DebugName; }

    inline RenderTarget* GetRenderTarget() { return _SceneRenderTarget.get(); }

    virtual void Load() {}

  protected: // Functions

    virtual void Initialize();

    void CreateSceneTexture();
    void CreateSceneDepthTexture();

  protected: // Fields
    std::string _DebugName;
    ScreenRectangle _ScreenRectangle;
    Game* _Game;

    NotCopyableReference<GuiScene> _GuiScene;

    Reference<Texture> _SceneTexture;
    Reference<Texture> _SceneDepthTexture;
    Reference<RenderTarget> _SceneRenderTarget;

  };

}
