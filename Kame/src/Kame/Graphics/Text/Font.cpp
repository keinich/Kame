#include "kmpch.h"
#include "Font.h"
#include <Kame/Core/FileUtility.h>
#include <Kame/Graphics/GraphicsCore.h>
#include <Kame\Graphics\Text\Consolas.h>

namespace Kame {

  Font::Font() {}

  Font::~Font() {}

  bool Font::Load(const std::wstring& fileName) {

    LoadFromBinary(L"default", g_pconsola24, sizeof(g_pconsola24));
    return true;
    //std::shared_ptr<std::vector<byte> >  ba = FileUtility::ReadFileSync(fileName);

    //if (ba->size() == 0) {
    //  ERROR("Cannot open file %ls", fileName.c_str());
    //  return false;
    //}

    //LoadFromBinary(fileName.c_str(), ba->data(), ba->size());

    //return true;
  }

  void Font::LoadFromBinary(const wchar_t* fontName, const uint8_t* binary, const size_t binarySize) {
    (fontName);

    // We should at least use this to assert that we have a complete file
    (binarySize);

    struct FontHeader {
      char FileDescriptor[8];        // "SDFFONT\0"
      uint8_t  majorVersion;        // '1'
      uint8_t  minorVersion;        // '0'
      uint16_t borderSize;        // Pixel empty space border width
      uint16_t textureWidth;        // Width of texture buffer
      uint16_t textureHeight;        // Height of texture buffer
      uint16_t fontHeight;        // Font height in 12.4
      uint16_t advanceY;            // Line height in 12.4
      uint16_t numGlyphs;            // Glyph count in texture
      uint16_t searchDist;        // Range of search space 12.4
    };

    FontHeader* header = (FontHeader*)binary;
    _NormalizeXCoord = 1.0f / (header->textureWidth * 16);
    _NormalizeYCoord = 1.0f / (header->textureHeight * 16);
    _FontHeight = header->fontHeight;
    _FontLineSpacing = (float)header->advanceY / (float)header->fontHeight;
    _BorderSize = header->borderSize * 16;
    _AntialiasRange = (float)header->searchDist / header->fontHeight;
    uint16_t textureWidth = header->textureWidth;
    uint16_t textureHeight = header->textureHeight;
    uint16_t NumGlyphs = header->numGlyphs;

    const wchar_t* wcharList = (wchar_t*)(binary + sizeof(FontHeader));
    const Glyph* glyphData = (Glyph*)(wcharList + NumGlyphs);
    const void* texelData = glyphData + NumGlyphs;

    for (uint16_t i = 0; i < NumGlyphs; ++i)
      _GlyphByChar[wcharList[i]] = glyphData[i];

    _Texture.reset(GraphicsCore::CreateTexture());
    _Texture->Create(textureWidth, textureHeight, DXGI_FORMAT_R8_SNORM, texelData);
    //_Texture.Create(textureWidth, textureHeight, DXGI_FORMAT_R8_SNORM, texelData);

    //DEBUGPRINT("Loaded SDF font:  %ls (ver. %d.%d)", fontName, header->majorVersion, header->minorVersion);
  }

  const Glyph* Font::GetGlyph(wchar_t ch) const {
    auto it = _GlyphByChar.find(ch);
    return it == _GlyphByChar.end() ? nullptr : &it->second;
  }

}
