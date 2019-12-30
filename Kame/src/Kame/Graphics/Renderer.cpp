#include "kmpch.h"
#include "Renderer.h"

#include <Kame/Core/DebugUtilities.h>
#include "GraphicsCore.h"
#include <Kame/Platform/DirectX12/Graphics/Display.h> //TOD abstract this
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/RenderTarget.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Game/Game.h>
#include <Kame/Graphics/PostEffects/ToneMapping.h>
#include "Renderer3D.h"
#include "2D/Renderer2D.h"
#include "Text/TextRenderer.h"
#include "Text/TextRenderContext.h"
#include <Kame\Graphics\Text\FontManager.h>

namespace Kame {

  Renderer* Renderer::_Instance = nullptr;

  Renderer::Renderer() {
    _ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
    _SdrRenderProgram = GraphicsCore::CreateRenderProgramNc();
    _SdrRootSignature = GraphicsCore::CreateRenderProgramSignatureNc();
  }

  Renderer::~Renderer() {}

  void Renderer::Initialize(Display* targetDisplay) {
    _TargetDIsplay = targetDisplay;
    CreateSdrProgram();
  }

  void Renderer::Render(Game* game) {
    Reference<CommandList> commandList = GraphicsCore::BeginCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    FLOAT clearColor[] = { 1.0f, 0.4f, 0.7f, 1.0f };

    for (Layer* layer : game->GetLayerStack()) {

      // Render against the RenderTarget of the Layer
      RenderTarget* renderTarget = layer->GetRenderTarget();

      commandList->ClearTexture(renderTarget->GetTexture(AttachmentPoint::Color0), clearColor);
      commandList->ClearDepthStencilTexture(renderTarget->GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);
      commandList->SetRenderTarget(*renderTarget);


      commandList->SetViewport(renderTarget->GetViewport());
      commandList->SetScissorRect(_ScissorRect);

      Renderer3D::RenderScene(
        layer->GetActiveCamera(),
        commandList.get(),
        layer->GetScene()
      );


      /* TextRenderer::Get()->Render(commandList.get());*/

       // Calculate the Viewport for the Copying to the TargetDisplay according to the screenRectangle of the Layer
      ScreenRectangle screenRectangle = layer->GetScreenRectangle();
      commandList->SetRenderTarget(_TargetDIsplay->GetRenderTarget());
      D3D12_VIEWPORT finalVp = _TargetDIsplay->GetRenderTarget().GetViewport(
        DirectX::XMFLOAT2(screenRectangle.GetRelativeWidth(), screenRectangle.GetRelativeHeight()),
        DirectX::XMFLOAT2(screenRectangle.GetRelativeLeft(), screenRectangle.GetRelativeTop())
      );

      commandList->SetViewport(finalVp);


      // ToneMapping
      // Copy the Layer-Scene to the TargetDisplay
      commandList->SetViewport(finalVp);
      commandList->SetRenderProgram(_SdrRenderProgram.get());
      commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      commandList->SetGraphicsRootSignature(_SdrRootSignature.get());
      commandList->SetGraphics32BitConstants(0, ToneMapping::GetParameters1());
      commandList->SetShaderResourceViewTexture(1, 0, renderTarget->GetTexture(Color0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

      commandList->Draw(3);

      // 2D Test
      std::vector<ColoredRectangle> rectangles;
      ColoredRectangle rectangle1;
      rectangle1.X = 0.0f;
      rectangle1.Y = 0.0f;
      rectangle1.Width = 0.1f;
      rectangle1.Height = 0.1f;
      rectangle1.SetColor(Math::Float4(1.0f, 0.0f, 1.0f, 1.0f));
      rectangles.push_back(rectangle1);
      ColoredRectangle rectangle2;
      rectangle2.X = 0.5f;
      rectangle2.Y = 0.0f;
      rectangle2.Width = 0.1f;
      rectangle2.Height = 0.1f;
      rectangle2.SetColor(Math::Float4(0.0f, 1.0f, 1.0f, 0.9f));
      rectangles.push_back(rectangle2);
      Renderer2D::Get()->RenderColoredRectangles(commandList.get(), rectangles);

      // Text Test
      TextRenderContext textRenderContext(commandList.get(), finalVp.Width, finalVp.Height);
      textRenderContext.SetColor(Math::Float4(1.0f, 0.0f, 0.0f, 1.0f));
      textRenderContext.SetCursorPosition(-0.2f, 1.0f);
      textRenderContext.SetTextSize(34);
      textRenderContext.DrawString(L"Welcome to the amazing KAME Engine");

      textRenderContext.SetColor(Math::Float4(1.0f, 0.8f, 0.8f, 1.0f));
      textRenderContext.SetCursorPosition(-0.2f, 0.9f);
      textRenderContext.SetTextSize(24);
      textRenderContext.DrawString(L"It is supergreat !! :)");
      textRenderContext.SetCursorPosition(-0.2f, 0.8f);
      textRenderContext.DrawString(L"Dont you think?");

      Reference<TextRenderItem> tri1 = CreateReference<TextRenderItem>();
      Reference<TextRenderItem> tri2 = CreateReference<TextRenderItem>();
      Reference<TextRenderItem> tri3 = CreateReference<TextRenderItem>();
      Reference<TextRenderItem> tri4 = CreateReference<TextRenderItem>();
      Reference<TextRenderItem> tri5 = CreateReference<TextRenderItem>();

      Font* font = FontManager::GetFont(L"default");
      tri1->X = 0.0f;
      //tri1->Y = -0.5f;
      tri1->Font = font;
      tri1->Color = Math::Float4(1.0f, 0.0f, 0.0f, 1.0f);
      tri1->Size = 5.0f;
      tri1->Text = L"tri1";

      tri2->Font = font;
      tri2->X = 0.1f;
      //tri2->Y = -0.5f;
      tri2->Color = Math::Float4(1.0f, 0.0f, 0.0f, 1.0f);
      tri2->Size = 10.0f;
      tri2->Text = L"tri2";

      tri3->Font = font;
      tri3->X = 0.2f;
      //tri3->Y = -0.5f;
      tri3->Color = Math::Float4(0.0f, 1.0f, 0.0f, 1.0f);
      tri3->Size = 5.0f;
      tri3->Text = L"tri3";

      tri4->Font = font;
      tri4->X = 0.3f;
      //tri4->Y = -0.5f;
      tri4->Color = Math::Float4(0.0f, 1.0f, 0.0f, 1.0f);
      tri4->Size = 5.0f;
      tri4->Text = L"tri4";

      tri5->Font = font;
      tri5->X = 10.8f;
      //tri5->Y = -0.5f;
      tri5->Color = Math::Float4(0.0f, 1.0f, 0.0f, 1.0f);
      tri5->Size = 10.0f;
      tri5->Text = L"tri5";

      std::vector < Reference<TextRenderItem>> textRenderItems;
      textRenderItems.push_back(tri1);
      textRenderItems.push_back(tri2);
      textRenderItems.push_back(tri3);
      textRenderItems.push_back(tri4);
      textRenderItems.push_back(tri5);


      TextRenderer::Get()->RenderTextItems(commandList.get(), textRenderItems);
    }

    GraphicsCore::ExecuteCommandList(commandList);

    // Render GUI.
    //OnGUI();

    // Present
    _TargetDIsplay->Present();

  }

  void Renderer::CreateSdrProgram() {
    //Signature
    CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER1 rootParameters[2];
    rootParameters[0].InitAsConstants(sizeof(ToneMapping::TonemapParameters1) / 4, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampsSampler);

    _SdrRootSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    //Program
    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDRtoSDR_VS.cso", &vs));
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDRtoSDR_PS.cso", &ps));

    CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
    rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

    _SdrRenderProgram->SetRootSignature(_SdrRootSignature.get());
    _SdrRenderProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _SdrRenderProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _SdrRenderProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
    _SdrRenderProgram->SetRasterizer(rasterizerDesc);
    _SdrRenderProgram->SetRenderTargetFormats(_TargetDIsplay->GetRenderTarget().GetRenderTargetFormats());

    _SdrRenderProgram->Create();
  }

}
