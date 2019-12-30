#pragma once

namespace Kame {

  class GuiRenderContext;

  class GuiElement {

  public:

    inline float GetAbsoulteX() { return _AbsoluteX; }
    inline float GetAbsoulteY() { return _AbsoluteY; }
    inline float GetRelativeX() { return _RelativeX; }
    inline float GetRelativeY() { return _RelativeY; }

    inline void SetRelativeX(float x) { _RelativeX = x; }
    inline void SetRelativeY(float y) { _RelativeY = y; }

  protected: // Methdos

    virtual void Render(GuiRenderContext& renderContext) = 0;

  protected: // Fields

    float _AbsoluteX;
    float _AbsoluteY;

    float _RelativeX;
    float _RelativeY;

  };

}