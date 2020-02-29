#pragma once

#include <vector>

#include <Kame/Core/References.h>
#include "GuiElement.h"

namespace Kame {

  class GuiScene {

  public:

    void AddGuiElement(Reference<GuiElement> guiElement);

    inline std::vector<Reference<GuiElement>>& GetGuiElements() { return _GuiElements; }

    void Update(UpdateEventArgs& e);

  private:

    std::vector < Reference<GuiElement>> _GuiElements;

  };

}