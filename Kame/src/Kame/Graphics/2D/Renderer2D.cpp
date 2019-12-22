#include "kmpch.h"
#include "Renderer2D.h"
#include <Kame/Core/DebugUtilities.h>
#include "Shaders/ColoredRectangle_PS.h"
#include "Shaders/ColoredRectangle_VS.h"
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/GraphicsCore.h>

namespace Kame {

  Renderer2D* Renderer2D::_Instance = nullptr;

  void Renderer2D::Create() {
    _Instance = new Renderer2D();
  }

  void Renderer2D::Destroy() {
    delete _Instance;
    _Instance = nullptr;
  }

  void Renderer2D::Initialize() {
    //Signature

    //CD3DX12_ROOT_PARAMETER1 rootParameters[];
    //rootParameters[0].InitAsShaderResourceView(
    //  0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL
    //);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(0, 0, 0, 0, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    _ColoredRectangleSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    //Program

    D3D12_INPUT_ELEMENT_DESC vertElem[] =
    {
        { "POSITION",    0, DXGI_FORMAT_R32G32_FLOAT     , 0, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "SCALE",       0, DXGI_FORMAT_R32G32_FLOAT     , 0, 8, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT     , 0, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 }
    };

    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    //vs = { g_ColoredRectangle_VS, sizeof(g_ColoredRectangle_VS) };
    //ps = g_ColoredRectangle_PS;
    //ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Text_VS.cso", &vs));
    //ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Text_PS.cso", &ps));

    CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    rasterizerDesc.FrontCounterClockwise = TRUE;

    _ColoredRectangleProgram->SetInputLayout(_countof(vertElem), vertElem);

    _ColoredRectangleProgram->SetRootSignature(_ColoredRectangleSignature.get());
    _ColoredRectangleProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    //_ColoredRectangleProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _ColoredRectangleProgram->SetVertexShader({ g_ColoredRectangle_VS, sizeof(g_ColoredRectangle_VS) });
    //_ColoredRectangleProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
    _ColoredRectangleProgram->SetPixelShader({ g_ColoredRectangle_PS, sizeof(g_ColoredRectangle_PS) });
    _ColoredRectangleProgram->SetRasterizer(rasterizerDesc);
    D3D12_RT_FORMAT_ARRAY rtFormat = {}; //TODO Woher kriegen wenn nicht stehlen?
    rtFormat.NumRenderTargets = 1;
    rtFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT dsFormat = DXGI_FORMAT_UNKNOWN;
    _ColoredRectangleProgram->SetRenderTargetFormats(rtFormat);

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

    _ColoredRectangleProgram->SetBlendDescription(blendDesc);

    _ColoredRectangleProgram->Create();
  }

  void Renderer2D::RenderColoredRectangles(CommandList* commandList, std::vector<ColoredRectangle>& rectangles) {
    commandList->SetGraphicsRootSignature(_ColoredRectangleSignature.get());
    commandList->SetRenderProgram(_ColoredRectangleProgram.get());
    commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    commandList->SetDynamicVertexBuffer(0, rectangles);
    commandList->Draw(4, rectangles.size());

  }

  Renderer2D::Renderer2D() {
    _ColoredRectangleSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    _ColoredRectangleProgram = GraphicsCore::CreateRenderProgramNc();
  }
  Renderer2D::~Renderer2D() {}

}