#include "kmpch.h"
#include "GuiRenderContext.h"

namespace Kame {

  void GuiRenderContext::Finish() {
    Context2D.Finish();
    TextContext.Finish();
  }

}