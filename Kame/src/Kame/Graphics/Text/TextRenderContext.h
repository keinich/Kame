#pragma once

#include <Kame/Math/Vector4.h>

namespace Kame {

  __declspec(align(16)) struct VertexShaderParams {
    Math::Vector4 ViewportTransform;
    float NormalizeX, NormalizeY, TextSize;
    float Scale, DstBorder;
    uint32_t SrcBorder;
  };

  __declspec(align(16)) struct PixelShaderParams {
    DirectX::XMVECTORF32 TextColor;
    float ShadowOffsetX, ShadowOffsetY;
    float ShadowHardness;        // More than 1 will cause aliasing
    float ShadowOpacity;        // Should make less opaque when making softer
    float HeightRange;

    PixelShaderParams() :
      TextColor(DirectX::g_XMOne),
      ShadowOffsetX(0.0f),
      ShadowOffsetY(0.0f),
      ShadowHardness(0.0f),
      ShadowOpacity(0.0f),
      HeightRange(0.0f) {}
  };

  class Font;
  class CommandList;

  class TextRenderContext {

    friend class TextRenderer;

  public:

    TextRenderContext(CommandList* commandList, float viewportWidth, float viewportHeight);

    inline const VertexShaderParams& GetVertexShaderParams() { return _VsParams; }
    inline const PixelShaderParams& GetPixelShaderParams() { return _PsParams; }

    inline const Font* GetCurrentFont() { return _CurrentFont; }

    virtual ~TextRenderContext();

    void DrawString(const std::wstring text, const float x = 0, const float y = 0);

    void SetColor(Math::Float4& color);
    void SetCursorPosition(float x, float y);
    void SetTextSize(float size);

  protected: // Methods

    void SetFont(const std::wstring& fontName, float TextSize = 0.0f);

    void SetFontDependentParameters(float size);

    void ResetSettings();
    void ResetCursor(float x, float y);

    void SetParameters();

  private: // Fields

    CommandList* _CommandList;

    VertexShaderParams _VsParams;
    PixelShaderParams _PsParams;

    const Font* _CurrentFont;

    bool _VsConstantBufferIsStale;    // Tracks when the CB needs updating
    bool _PsConstantBufferIsStale;    // Tracks when the CB needs updating
    bool _TextureIsStale;
    bool _EnableShadow;
    float _LeftMargin;
    float _TextPosX;
    float _TextPosY;
    float _LineHeight;
    float _ViewWidth;                // Width of the drawable area
    float _ViewHeight;                // Height of the drawable area
    float _ShadowOffsetX;            // Percentage of the font's TextSize should the shadow be offset
    float _ShadowOffsetY;            // Percentage of the font's TextSize should the shadow be offset
  };

}