#pragma once

#include "GuiElement.h"

#include <Kame/Graphics/2D/ColoredRectangle.h>
#include <Kame/Graphics/Text/TextRenderItem.h>

namespace Kame {

  class Label : public GuiElement {

  public:

    Label();
    virtual ~Label();

    void SetText(std::wstring text);
    std::wstring GetText();

  protected: // Methods

    virtual void Render(GuiRenderContext& renderContext) override;


  private:

    ColoredRectangle _ColoredRectangle;
    TextRenderItem _TextRenderItem;

  };

}