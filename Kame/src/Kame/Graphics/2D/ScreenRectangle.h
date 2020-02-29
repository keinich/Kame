#pragma once

namespace Kame {

  class ScreenRectangle {

  public:

    ScreenRectangle() {
      _RelativeLeft = 0;
      _RelativeTop = 0;
      _RelativeWidth = 1;
      _RelativeHeight = 1;
      _AbsoluteLeft = 0;
      _AbsoluteTop = 0;
      _AbsoluteWidth = 0;
      _AbsoluteHeight = 0;
    }

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
    UINT GetAbsoluteHeight() const;

    float GetRelativeLeft() { return _RelativeLeft; }
    float GetRelativeTop() { return _RelativeTop; }
    float GetRelativeWidth() { return _RelativeWidth; }
    float GetRelativeHeight() { return _RelativeHeight; }

    inline void SetAbsoluteWidth(UINT64 width) { _AbsoluteWidth = width; }
    inline void SetAbsoluteHeight(UINT height) { _AbsoluteHeight = height; }

    inline void SetRelativeLeft(float left) { _RelativeLeft = left; }
    inline void SetRelativeTop(float top) { _RelativeTop = top; }
    inline void SetRelativeWidth(float width) { _RelativeWidth = width; }
    inline void SetRelativeHeight(float height) { _RelativeHeight = height; }

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

}