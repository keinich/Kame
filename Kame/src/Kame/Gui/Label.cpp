#include "kmpch.h"
#include "Label.h"
#include "GuiRenderContext.h"

namespace Kame {

  Label::Label() {
    _TextRenderItem = CreateReference<TextRenderItem>();
  }

  Label::~Label() {}

  void Label::SetText(std::wstring text) {
    _TextRenderItem->Text = text;
  }

  std::wstring Label::GetText() {
    return _TextRenderItem->Text;
  }

  void Label::Render(GuiRenderContext& renderContext) {
    renderContext.Context2D.SubmitColoredRectangle(_ColoredRectangle);
    renderContext.TextContext.SubmitTextRenderItem(_TextRenderItem);
  }

  void Label::OnScreenRectangleChanged() {
    _ColoredRectangle.X = _ScreenRectangle.GetRelativeLeft();
    _ColoredRectangle.Y = _ScreenRectangle.GetRelativeTop();
    _ColoredRectangle.Width = _ScreenRectangle.GetRelativeWidth();
    _ColoredRectangle.Height = _ScreenRectangle.GetRelativeHeight();

    _TextRenderItem->X = _ScreenRectangle.GetRelativeLeft();
    _TextRenderItem->Y = _ScreenRectangle.GetRelativeTop();
    
    /*_ColoredRectangle.X = 0.5f;
    _ColoredRectangle.Y = 0.5f;
    _TextRenderItem->X = 0.5f;
    _TextRenderItem->Y = 0.5f;*/
    
  }

}

