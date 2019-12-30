#include "kmpch.h"
#include "Label.h"
#include "GuiRenderContext.h"

namespace Kame {

  Label::Label() {}

  Label::~Label() {}

  void Label::SetText(std::wstring text) {
    _TextRenderItem.Text = text;
  }

  std::wstring Label::GetText() {
    return _TextRenderItem.Text;
  }

  void Label::Render(GuiRenderContext& renderContext) {
    renderContext.Context2D.SubmitColoredRectangle(_ColoredRectangle);
  }

}

