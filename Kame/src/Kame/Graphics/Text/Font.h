#pragma once

#include <Kame/Core/References.h>
#include <Kame/Graphics/RenderApi/Texture.h>

namespace Kame {

  struct Glyph {
    uint16_t x, y, w;
    int16_t bearing;
    uint16_t advance;
  };

  class Font {

  public:
    Font();
    virtual ~Font();

    bool Load(const std::wstring& fileName);
    void LoadFromBinary(const wchar_t* fontName, const uint8_t* binary, const size_t binarySize);

    inline const Texture* GetTexture() const { return _Texture.get(); }

    const Glyph* GetGlyph(wchar_t ch) const;

    // Get the texel height of the font in 12.4 fixed point
    uint16_t GetHeight(void) const { return _FontHeight; }

    // Get the size of the border in 12.4 fixed point
    uint16_t GetBorderSize(void) const { return _BorderSize; }

    // Get the line advance height given a certain font size
    float GetVerticalSpacing(float size) const { return size * _FontLineSpacing; }

    float GetXNormalizationFactor() const { return _NormalizeXCoord; }
    float GetYNormalizationFactor() const { return _NormalizeYCoord; }

    // Get the range in terms of height values centered on the midline that represents a pixel
    // in screen space (according to the specified font size.)
    // The pixel alpha should range from 0 to 1 over the height range 0.5 +/- 0.5 * aaRange.
    float GetAntialiasRange(float size) const { return std::max(1.0f, size * _AntialiasRange); }

  private:

    NotCopyableReference<Texture> _Texture;

    float _NormalizeXCoord;
    float _NormalizeYCoord;
    float _FontLineSpacing;
    float _AntialiasRange;
    uint16_t _FontHeight;
    uint16_t _BorderSize;
    uint16_t _TextureWidth;
    uint16_t _TextureHeight;
    std::map<wchar_t, Glyph> _GlyphByChar;

  };

}