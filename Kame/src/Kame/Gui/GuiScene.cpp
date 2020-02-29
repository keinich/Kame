#include "kmpch.h"
#include "GuiScene.h"

namespace Kame {

  void GuiScene::AddGuiElement(Reference<GuiElement> guiElement) {
    _GuiElements.push_back(guiElement);
  }

  void GuiScene::Update(UpdateEventArgs& e) {
    for (Reference<GuiElement> guiElement : _GuiElements) {
      guiElement->Update(e);
    }
  }

}
