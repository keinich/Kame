#pragma once

#include "Kame/Core.h"
#include "Kame/Events/Event.h"

namespace Kame {

  class KAME_API Layer {
    public:
    Layer(const std::string& name = "Layer");
    ~Layer();

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnEvent(Event& event) {}

    inline const std::string& GetName() const { return _DebugName; }

    protected:
    std::string _DebugName;
  };

}
