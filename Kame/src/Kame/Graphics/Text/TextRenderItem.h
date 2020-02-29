#pragma once
#include <string>
#include <Kame/Math/Vector4.h>
#include <Kame/Graphics/Text/Font.h>

namespace Kame {

  struct TextRenderItem {

    TextRenderItem() {
      Text = L"Text Render Item";
      X = 0;
      Y = 0;
      Size = 20;
      Color = Math::Float4(1.0f, 0.5f, 0.4f, 1.0f);
    }

    std::wstring Text;
    float X;
    float Y;
    float Size;
    Math::Float4 Color;
    Font* Font;

  };

}