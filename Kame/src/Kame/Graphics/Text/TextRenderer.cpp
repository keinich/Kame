#include "kmpch.h"
#include "TextRenderer.h"

#include <Kame/Core/DebugUtilities.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/GraphicsCore.h>
#include <Kame/Graphics/RenderApi/CommandList.h>

#include <Kame/Graphics/PostEffects/ToneMapping.h>
#include "Font.h"
#include "FontManager.h"

namespace Kame {

  TextRenderer* TextRenderer::s_Instance = nullptr;

  void TextRenderer::Create() {
    s_Instance = new TextRenderer();
  }

  void TextRenderer::Destroy() {
    delete s_Instance;
    s_Instance = nullptr;
  }

  TextRenderer::TextRenderer() {
    _ProgramSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    _Program = GraphicsCore::CreateRenderProgramNc();
  }

  TextRenderer::~TextRenderer() {}

  void TextRenderer::Initialize() {

    //Signature
    CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER1 rootParameters[3];
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &linearClampsSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    _ProgramSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    //Program

    D3D12_INPUT_ELEMENT_DESC vertElem[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT     , 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "TEXCOORD", 0, DXGI_FORMAT_R16G16B16A16_UINT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
    };

    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Text_VS.cso", &vs));
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Text_PS.cso", &ps));

    CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FrontCounterClockwise = TRUE;

    _Program->SetInputLayout(_countof(vertElem), vertElem);

    _Program->SetRootSignature(_ProgramSignature.get());
    _Program->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _Program->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _Program->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
    _Program->SetRasterizer(rasterizerDesc);
    D3D12_RT_FORMAT_ARRAY rtFormat = {}; //TODO Woher kriegen wenn nicht stehlen?
    rtFormat.NumRenderTargets = 1;
    rtFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT dsFormat = DXGI_FORMAT_UNKNOWN;
    _Program->SetRenderTargetFormats(rtFormat);

    CD3DX12_BLEND_DESC blendDesc;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;

    blendDesc.RenderTarget[0].LogicOpEnable = FALSE;

    _Program->SetBlendDescription(blendDesc);

    _Program->Create();

    SetFont(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\GOBOLD", 1.0f);

  }

  __declspec(align(16)) struct TextVert {
    float X, Y;                // Upper-left glyph position in screen space
    uint16_t U, V, W, H;    // Upper-left glyph UV and the width in texture space
  };

  void TextRenderer::Render(CommandList* commandList, const std::wstring text) {

    //m_CurrentFont = nullptr;
    _ViewWidth = 200;
    _ViewHeight = 240;

    // Transform from text view space to clip space.
    const float vpX = -1.0f;
    const float vpY = 0.0f;
    const float twoDivW = 2.0f / _ViewWidth;
    const float twoDivH = 2.0f / _ViewHeight;
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, -vpX * twoDivW - 1.0f, vpY * twoDivH + 1.0f);
    _VsParams.ViewportTransform = Math::Vector4(twoDivW, -twoDivH, -1.0f, 0.5f);

    // The font texture dimensions are still unknown
    //_VsParams.NormalizeX = 1.0f;
    //_VsParams.NormalizeY = 1.0f;

    ResetSettings();

    commandList->SetRenderProgram(_Program.get());
    commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->SetGraphicsRootSignature(_ProgramSignature.get());

    //_PsParams.TextColor = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    //g_vsParams.TextSize = 10.0f;

    commandList->SetGraphicsDynamicConstantBuffer(0, _VsParams);
    commandList->SetGraphicsDynamicConstantBuffer(1, _PsParams);
    //commandList->SetShaderResourceViewTexture(2, 0, tex, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->SetShaderResourceViewTexture(2, 0, m_CurrentFont->GetTexture(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    std::vector<TextVert> vertices;
    TextVert vertex;
    vertex.X = 0.0f;
    vertex.Y = 0.0f;
    vertex.U = 0.0f;
    vertex.V = 0.0f;
    vertex.W = 1.0f / _VsParams.NormalizeX;
    vertex.H = 1.0f / _VsParams.NormalizeY;
    //vertex.W = 1.0f;
    //vertex.H = 1.0f;
    TextVert vertex2;
    vertex2.X = 10.3f;
    vertex2.Y = 10.4f;
    vertex2.U = 0.0f;
    vertex2.V = 0.0f;
    vertex2.W = 1.0f / _VsParams.NormalizeX;
    vertex2.H = 1.0f / _VsParams.NormalizeY;
    //vertex2.W = 1.0f;
    //vertex2.H = 1.0f;

    float xMargin = 20.0f;
    float currentX = 0.0f;
    for (wchar_t c : text) {

      const Glyph* glyph = m_CurrentFont->GetGlyph(c);
      TextVert vertex;
      vertex.X = currentX;
      vertex.Y = 0.0f;
      vertex.U = glyph->x;
      vertex.V = glyph->y;
      vertex.W = glyph->w;
      vertex.H = m_CurrentFont->GetHeight();;
      vertices.push_back(vertex);

      currentX += xMargin;
    }

    //vertices.push_back(vertex);

    commandList->SetDynamicVertexBuffer(0, vertices);

    commandList->Draw(4, vertices.size());
  }

  void TextRenderer::SetFont(const std::wstring& fontName, float size) {
    m_CurrentFont = FontManager::GetFont(fontName);

    if (size > 0.0f)
      _VsParams.TextSize = size;

    // Update constants directly tied to the font or the font size
    _LineHeight = m_CurrentFont->GetVerticalSpacing(_VsParams.TextSize);
    _VsParams.NormalizeX = m_CurrentFont->GetXNormalizationFactor();
    _VsParams.NormalizeY = m_CurrentFont->GetYNormalizationFactor();
    _VsParams.Scale = _VsParams.TextSize / m_CurrentFont->GetHeight();
    _VsParams.DstBorder = m_CurrentFont->GetBorderSize() * _VsParams.Scale;
    _VsParams.SrcBorder = m_CurrentFont->GetBorderSize();
    _PsParams.ShadowOffsetX = m_CurrentFont->GetHeight() * _ShadowOffsetX * _VsParams.NormalizeX;
    _PsParams.ShadowOffsetY = m_CurrentFont->GetHeight() * _ShadowOffsetY * _VsParams.NormalizeY;
    _PsParams.HeightRange = m_CurrentFont->GetAntialiasRange(_VsParams.TextSize);
    _VSConstantBufferIsStale = true;
    _PSConstantBufferIsStale = true;
    _TextureIsStale = true;
  }

  void TextRenderer::ResetSettings(void) {
    _EnableShadow = true;
    ResetCursor(0.0f, 0.0f);
    _ShadowOffsetX = 0.05f;
    _ShadowOffsetY = 0.05f;
    _PsParams.ShadowHardness = 0.5f;
    _PsParams.ShadowOpacity = 1.0f;
    //_PsParams.TextColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

    _VSConstantBufferIsStale = true;
    _PSConstantBufferIsStale = true;
    _TextureIsStale = true;

    SetFont(L"default", 24.0f);
  }

  void TextRenderer::ResetCursor(float x, float y) {
    _LeftMargin = x;
    _TextPosX = x;
    _TextPosY = y;
  }
}