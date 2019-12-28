#pragma once
#include <string>
#include <Kame/Math/Vector4.h>
#include <Kame/Graphics/Text/Font.h>

namespace Kame {

  struct TextRenderItem {

    std::wstring Text;
    float X;
    float Y;
    float Size;
    Math::Float4 Color;
    Font* Font;

  };

}