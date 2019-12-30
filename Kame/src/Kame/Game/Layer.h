#pragma once

#include "Kame/Events/Event_Cherno.h"

#include <Kame/Core/References.h>
#include <Kame\Events\EventArgs.h>

namespace Kame {

  class Camera;
  class Scene3D;
  class RenderTarget;
  class Texture;
  class Game;

  class ScreenRectangle {

  public:

    ScreenRectangle(
      ScreenRectangle& parentRectangle,
      float relativeLeft,
      float relativeTop,
      float relativeWidth,
      float relativeHeight
    ) {
      _RelativeLeft = relativeLeft;
      _RelativeTop = relativeTop;
      _RelativeWidth = relativeWidth;
      _RelativeHeight = relativeHeight;
      _AbsoluteLeft = 0;
      _AbsoluteTop = 0;
      _AbsoluteWidth = 0;
      _AbsoluteHeight = 0;
      UpdateAbsoluteValues(parentRectangle);
    }

    ScreenRectangle(
      UINT parentLeft, UINT parentTop, UINT64 parentWidth, UINT parentHeight,
      float relativeLeft,
      float relativeTop,
      float relativeWidth,
      float relativeHeight
    ) {
      _RelativeLeft = relativeLeft;
      _RelativeTop = relativeTop;
      _RelativeWidth = relativeWidth;
      _RelativeHeight = relativeHeight;
      _AbsoluteLeft = 0;
      _AbsoluteTop = 0;
      _AbsoluteWidth = 0;
      _AbsoluteHeight = 0;
      UpdateAbsoluteValues(parentLeft, parentTop, parentWidth, parentHeight);
    }

    ScreenRectangle(
      UINT absoluteLeft,
      UINT absoluteTop,
      UINT64 absoluteWidth,
      UINT absoluteHeight
    ) {
      _AbsoluteLeft = absoluteLeft;
      _AbsoluteTop = absoluteTop;
      _AbsoluteWidth = absoluteWidth;
      _AbsoluteHeight = absoluteHeight;
      _RelativeLeft = 0;
      _RelativeTop = 0;
      _RelativeWidth = 1;
      _RelativeHeight = 1;
    }

    void UpdateAbsoluteValues(
      UINT parentLeft, UINT parentTop, UINT64 parentWidth, UINT parentHeight
    );

    void UpdateAbsoluteValues(
      ScreenRectangle& parentRectangle
    );

    UINT GetAbsoluteLeft();
    UINT GetAbsoluteTop();
    UINT64 GetAbsoluteWidth() const;
    UINT GetAbsoluteHeight() const ;

    float GetRelativeLeft() { return _RelativeLeft; }
    float GetRelativeTop() { return _RelativeTop; }
    float GetRelativeWidth() { return _RelativeWidth; }
    float GetRelativeHeight() { return _RelativeHeight; }

    inline void SetAbsoluteWidth(UINT64 width) { _AbsoluteWidth = width; }
    inline void SetAbsoluteHeight(UINT height) { _AbsoluteHeight = height; }

  private:

    float _RelativeTop;
    float _RelativeLeft;
    float _RelativeWidth;
    float _RelativeHeight;

    UINT _AbsoluteTop;
    UINT _AbsoluteLeft;
    UINT64 _AbsoluteWidth;
    UINT _AbsoluteHeight;

  };

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
    Game* _Game;

    Reference<Texture> _SceneTexture;
    Reference<Texture> _SceneDepthTexture;
    Reference<RenderTarget> _SceneRenderTarget;

  };

}
