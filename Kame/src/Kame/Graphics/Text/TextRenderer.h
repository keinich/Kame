#pragma once

#include <Kame/Core/References.h>
#include <Kame/Math/Vector4.h>

namespace Kame {

  class RenderProgramSignature;
  class RenderProgram;
  class CommandList;
  class Texture; //TODO Remove this
  class Font;

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

 
  class TextRenderer {

  public:

    static void Create();
    static void Destroy();

    inline static TextRenderer* Get() { return s_Instance; }

    void Initialize();

    void Render(CommandList* commandList, const std::wstring text);

    void SetFont(const std::wstring& fontName, float TextSize = 0.0f);

    void ResetSettings();
    void ResetCursor(float x, float y);

  private: // Methods
    TextRenderer();
    virtual ~TextRenderer();

  private: // Fields

    static TextRenderer* s_Instance;

    VertexShaderParams _VsParams;
    PixelShaderParams _PsParams;

    bool _VSConstantBufferIsStale;    // Tracks when the CB needs updating
    bool _PSConstantBufferIsStale;    // Tracks when the CB needs updating
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

    NotCopyableReference<RenderProgramSignature> _ProgramSignature;
    NotCopyableReference<RenderProgram> _Program;

    const Font* m_CurrentFont;
  };

}