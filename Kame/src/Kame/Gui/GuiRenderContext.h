#pragma once

#include <Kame/Graphics/2D/RenderContext2D.h>
#include <Kame/Graphics/Text/TextRenderContext.h>

namespace Kame {

  class CommandList;

  class GuiRenderContext {

  public:

    GuiRenderContext(CommandList* commandList, float viewportWidth, float viewportHeight) :
      Context2D(commandList),
      TextContext(commandList, viewportWidth, viewportHeight) {}

    virtual ~GuiRenderContext() {}

    void Finish();

    RenderContext2D Context2D;
    TextRenderContext TextContext;

  };

}