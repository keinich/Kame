#include "kmpch.h"
#include "RenderContext2D.h"

#include "ColoredRectangle.h"
#include "Renderer2D.h"

namespace Kame {

  RenderContext2D::RenderContext2D(CommandList* commandList) {
    _CommandList = commandList;
  }

  RenderContext2D::~RenderContext2D() {
    _Rectangles.clear();
  }

  void RenderContext2D::SubmitColoredRectangle(ColoredRectangle& coloredRectangle) {
    _Rectangles.push_back(coloredRectangle);
  }

  void RenderContext2D::Finish() {
    Renderer2D::Get()->RenderColoredRectangles(_CommandList, _Rectangles);
  }

}