#pragma once

#include <Kame/Math/Vector4.h>

namespace Kame {

  __declspec(align(16)) struct ColoredRectangle {

    ColoredRectangle() :
      X(0),
      Y(0),
      Width(1),
      Height(1),
      Color(DirectX::g_XMOne)
    {}

    float X;
    float Y;
    float Width;
    float Height;

    //Math::Float4 Color;
    DirectX::XMVECTORF32 Color;

    void SetColor(Math::Float4 color);
  };

}