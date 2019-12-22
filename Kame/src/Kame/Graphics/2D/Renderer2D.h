#pragma once

#include <Kame/Core/References.h>
#include "ColoredRectangle.h"

namespace Kame {

  class RenderProgram;
  class RenderProgramSignature;
  class CommandList;  

  class Renderer2D {

  public:

    static void Create();
    static void Destroy();

    inline static Renderer2D* Get() { return _Instance; }

    void Initialize();

    void RenderColoredRectangles(CommandList* commandList, std::vector<ColoredRectangle>& rectangles);

  private: // Methods

    Renderer2D();
    virtual ~Renderer2D();

  private: // Fields

    static Renderer2D* _Instance;

    NotCopyableReference<RenderProgram> _ColoredRectangleProgram;
    NotCopyableReference<RenderProgramSignature> _ColoredRectangleSignature;

  };

}