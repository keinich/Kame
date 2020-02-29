#include "kmpch.h"
#include "ScreenRectangle.h"

namespace Kame {

  void ScreenRectangle::UpdateAbsoluteValues(
    UINT parentLeft, UINT parentTop, UINT64 parentWidth, UINT parentHeight
  ) {
    _AbsoluteLeft = parentLeft + _RelativeLeft * parentWidth;
    _AbsoluteTop = parentTop + _RelativeTop * parentHeight;
    _AbsoluteWidth = _RelativeWidth * parentWidth;
    _AbsoluteHeight = _RelativeHeight * parentHeight;
  }

  void ScreenRectangle::UpdateAbsoluteValues(ScreenRectangle& parentRectangle) {
    UpdateAbsoluteValues(
      parentRectangle.GetAbsoluteLeft(),
      parentRectangle.GetAbsoluteTop(),
      parentRectangle.GetAbsoluteWidth(),
      parentRectangle.GetAbsoluteHeight()
    );
  }

  UINT ScreenRectangle::GetAbsoluteLeft() {
    return _AbsoluteLeft;
  }

  UINT ScreenRectangle::GetAbsoluteTop() {
    return _AbsoluteTop;
  }

  UINT64 ScreenRectangle::GetAbsoluteWidth() const {
    return _AbsoluteWidth;
  }

  UINT ScreenRectangle::GetAbsoluteHeight() const {
    return _AbsoluteHeight;
  }

}