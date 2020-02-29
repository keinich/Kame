#include "kmpch.h"
#include "GuiElement.h"

namespace Kame {

  void GuiElement::SetRelativeLeft(float left) {
    _ScreenRectangle.SetRelativeLeft(left);
    _ScreenRectangleIsDirty = true;
  }

  void GuiElement::SetRelativeTop(float top) {
    _ScreenRectangle.SetRelativeTop(top);
    _ScreenRectangleIsDirty = true;
  }

  void GuiElement::SetRelativeWidth(float width) {
    _ScreenRectangle.SetRelativeWidth(width);
    _ScreenRectangleIsDirty = true;
  }

  void GuiElement::SetRelativeHeight(float height) {
    _ScreenRectangle.SetRelativeHeight(height);
    _ScreenRectangleIsDirty = true;
  }

  void GuiElement::Update(UpdateEventArgs& e) {
    if (_ScreenRectangleIsDirty) {
      OnScreenRectangleChanged();
      _ScreenRectangleIsDirty = false;
    }
  }

}
