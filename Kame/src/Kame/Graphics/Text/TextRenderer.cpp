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
#include "TextRenderContext.h"

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

  void TextRenderer::Begin(CommandList* commandList) {
    commandList->SetRenderProgram(_Program.get());
    commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->SetGraphicsRootSignature(_ProgramSignature.get());
  }

  void TextRenderer::Initialize() {

    //Signature
    CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER1 rootParameters[3];
    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

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

  }

}