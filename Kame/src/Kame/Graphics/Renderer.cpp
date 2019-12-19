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
#include "Text/TextRenderer.h"
#include "Text/TextRenderContext.h"

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
        DirectX::XMFLOAT2(screenRectangle.Width, screenRectangle.Height),
        DirectX::XMFLOAT2(screenRectangle.Left, screenRectangle.Top)
      );

      commandList->SetViewport(finalVp);

      // Copy the Layer-Scene to the TargetDisplay
      commandList->SetViewport(finalVp);
      commandList->SetRenderProgram(_SdrRenderProgram.get());
      commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      commandList->SetGraphicsRootSignature(_SdrRootSignature.get());
      commandList->SetGraphics32BitConstants(0, ToneMapping::GetParameters1());
      commandList->SetShaderResourceViewTexture(1, 0, renderTarget->GetTexture(Color0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

      commandList->Draw(3);

      TextRenderContext textRenderContext = TextRenderContext::Begin(commandList.get(), finalVp.Width, finalVp.Height);
      textRenderContext.SetColor(Math::Float4(1.0f, 0.0f, 0.0f, 1.0f));
      textRenderContext.SetCursorPosition(-0.2f, 1.0f);
      textRenderContext.DrawString(L"Welcome to the amazing KAME Engine");

      textRenderContext.SetColor(Math::Float4(1.0f, 0.8f, 0.8f, 1.0f));
      textRenderContext.SetCursorPosition(-0.2f, 0.9f);
      textRenderContext.SetTextSize(12);
      textRenderContext.DrawString(L"It is supergreat !! :)");
      textRenderContext.SetCursorPosition(-0.2f, 0.8f);
      textRenderContext.DrawString(L"Dont you think?");
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
