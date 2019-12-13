#pragma once

#include "Layer.h"

#include <vector>

namespace Kame {

  class KAME_API LayerStack {

  public:
    LayerStack();
    ~LayerStack();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* overlay);
    void PopLayer(Layer* layer);
    void PopOverlay(Layer* overlay);

    std::vector<Layer*>::iterator begin() { return _Layers.begin(); }
    std::vector<Layer*>::iterator end() { return _Layers.end(); }

  private:
    std::vector<Layer*> _Layers;
    std::vector<Layer*>::iterator _LayerInsert;
  };

}
