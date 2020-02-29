#include "kmpch.h"
#include "TextRenderContext.h"
#include "FontManager.h"
#include "TextRenderer.h"
#include <Kame/Graphics/RenderApi/CommandList.h>

namespace Kame {

  TextRenderContext::TextRenderContext(CommandList* commandList, float viewportWidth, float viewportHeight) {

    _CommandList = commandList;

    TextRenderer::Get()->Begin(commandList);

    _PsParams.TextColor.f[0] = 1.0f;
    _PsParams.TextColor.f[1] = 1.0f;
    _PsParams.TextColor.f[2] = 1.0f;
    _PsParams.TextColor.f[3] = 1.0f;

    //m_CurrentFont = nullptr;
    _ViewWidth = viewportWidth;
    _ViewHeight = viewportHeight;

    // Transform from text view space to clip space.
    const float vpX = -1.0f;
    const float vpY = 0.0f;
    const float twoDivW = 2.0f / _ViewWidth;
    const float twoDivH = 2.0f / _ViewHeight;
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, -vpX * twoDivW - 1.0f, vpY * twoDivH + 1.0f);
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, -1.0f, 0.5f);

    ResetSettings();

  }

  TextRenderContext::~TextRenderContext() {
    _TextRenderItems.clear();
  }

  void TextRenderContext::DrawString(const std::wstring text, const float x, const float y) {

    SetParameters();

    std::vector<TextVert> vertices;

    float xMargin = 20.0f;
    const float UVtoPixel = _VsParams.Scale;
    //float currentX = x / UVtoPixel;
    float currentX = x;
    //float currentY = y / UVtoPixel;
    float currentY = y;

    for (wchar_t c : text) {

      const Glyph* glyph = _CurrentFont->GetGlyph(c);
      TextVert vertex;
      vertex.X = currentX;
      vertex.Y = currentY;
      vertex.U = glyph->x;
      vertex.V = glyph->y;
      vertex.W = glyph->w;
      vertex.H = _CurrentFont->GetHeight();;
      vertices.push_back(vertex);

      //currentX += xMargin;
      currentX += (float)glyph->advance * UVtoPixel;

    }

    _CommandList->SetDynamicVertexBuffer(0, vertices);

    _CommandList->Draw(4, vertices.size());
  }

  void TextRenderContext::SetColor(Math::Float4& color) {
    _PsParams.TextColor.f[0] = color.x;
    _PsParams.TextColor.f[1] = color.y;
    _PsParams.TextColor.f[2] = color.z;
    _PsParams.TextColor.f[3] = color.w;

    _PsConstantBufferIsStale = true;
  }

  void TextRenderContext::SetCursorPosition(float x, float y) {
    const float vpX = x;
    const float vpY = y;
    const float twoDivW = 2.0f / _ViewWidth;
    const float twoDivH = 2.0f / _ViewHeight;
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, -vpX * twoDivW - 1.0f, vpY * twoDivH + 1.0f);
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, x, y);

    _VsConstantBufferIsStale = true;
  }

  void TextRenderContext::SetTextSize(float size) {
    SetFontDependentParameters(size);
    _VsConstantBufferIsStale = true;
  }

  void TextRenderContext::SubmitTextRenderItem(Reference<TextRenderItem>& tri) {
    _TextRenderItems.push_back(tri);
  }

  void TextRenderContext::Finish() {
    TextRenderer::Get()->RenderTextItems(_CommandList, _TextRenderItems, _ViewWidth, _ViewHeight);
  }

  void TextRenderContext::SetParameters() {
    if (_VsConstantBufferIsStale) {
      _CommandList->SetGraphicsDynamicConstantBuffer(0, _VsParams);
      _VsConstantBufferIsStale = false;
    }
    if (_PsConstantBufferIsStale) {
      _CommandList->SetGraphicsDynamicConstantBuffer(1, _PsParams);
      _PsConstantBufferIsStale = false;
    }
    if (_TextureIsStale) {
      _CommandList->SetShaderResourceViewTexture(2, 0, _CurrentFont->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
      _TextureIsStale = false;
    }
  }

  void TextRenderContext::ResetSettings(void) {
    _EnableShadow = true;
    ResetCursor(0.0f, 0.0f);
    _ShadowOffsetX = 0.05f;
    _ShadowOffsetY = 0.05f;
    _PsParams.ShadowHardness = 0.5f;
    _PsParams.ShadowOpacity = 1.0f;
    //_PsParams.TextColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

    _VsConstantBufferIsStale = true;
    _PsConstantBufferIsStale = true;
    _TextureIsStale = true;

    SetFont(L"default", 24.0f);
  }

  void TextRenderContext::ResetCursor(float x, float y) {
    _LeftMargin = x;
    _TextPosX = x;
    _TextPosY = y;
  }

  void TextRenderContext::SetFont(const std::wstring& fontName, float size) {
    _CurrentFont = FontManager::GetFont(fontName);

    SetFontDependentParameters(size);
    _TextureIsStale = true;
  }

  void TextRenderContext::SetFontDependentParameters(float size) {
    if (size > 0.0f)
      _VsParams.TextSize = size;

    // Update constants directly tied to the font or the font size
    _LineHeight = _CurrentFont->GetVerticalSpacing(_VsParams.TextSize);
    _VsParams.NormalizeX = _CurrentFont->GetXNormalizationFactor();
    _VsParams.NormalizeY = _CurrentFont->GetYNormalizationFactor();
    _VsParams.Scale = _VsParams.TextSize / _CurrentFont->GetHeight();
    _VsParams.DstBorder = _CurrentFont->GetBorderSize() * _VsParams.Scale;
    _VsParams.SrcBorder = _CurrentFont->GetBorderSize();
    _PsParams.ShadowOffsetX = _CurrentFont->GetHeight() * _ShadowOffsetX * _VsParams.NormalizeX;
    _PsParams.ShadowOffsetY = _CurrentFont->GetHeight() * _ShadowOffsetY * _VsParams.NormalizeY;
    _PsParams.HeightRange = _CurrentFont->GetAntialiasRange(_VsParams.TextSize);
    _VsConstantBufferIsStale = true;
    _PsConstantBufferIsStale = true;
  }

}