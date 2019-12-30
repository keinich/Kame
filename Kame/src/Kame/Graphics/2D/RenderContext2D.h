#pragma once

#include <vector>

#include <Kame/Core/References.h>

namespace Kame {

  class ColoredRectangle;
  class CommandList;

  class RenderContext2D {

  public:

    RenderContext2D(CommandList* commandList);
    virtual ~RenderContext2D();

    void SubmitColoredRectangle(ColoredRectangle& coloredRectangle);

    void Finish();

  private:

    CommandList* _CommandList;
    std::vector<ColoredRectangle> _Rectangles;

  };

}