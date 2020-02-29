#pragma once

#include <Kame/Graphics/2D/ScreenRectangle.h>
#include <Kame\Events\EventArgs.h>

namespace Kame {

  class GuiRenderContext;

  class GuiElement {

    friend class Renderer;
    friend class GuiScene;

  public:

    //inline float GetAbsoulteX() { return _AbsoluteX; }
    //inline float GetAbsoulteY() { return _AbsoluteY; }
    //inline float GetRelativeX() { return _RelativeX; }
    //inline float GetRelativeY() { return _RelativeY; }

    void SetRelativeLeft(float left);
    void SetRelativeTop(float top);
    void SetRelativeWidth(float width);
    void SetRelativeHeight(float height);
    //inline void SetRelativeY(float y) { _RelativeY = y; }

  protected: // Methdos

    void Update(UpdateEventArgs& e);
    virtual void Render(GuiRenderContext& renderContext) = 0;

    virtual void OnScreenRectangleChanged() = 0;

  protected: // Fields

    ScreenRectangle _ScreenRectangle;
    bool _ScreenRectangleIsDirty;

  };

}