#include "kmpch.h"
#include "ColoredRectangle.h"

void Kame::ColoredRectangle::SetColor(Math::Float4 color) {
  Color.f[0] = color.x;
  Color.f[1] = color.y;
  Color.f[2] = color.z;
  Color.f[3] = color.w;
}
