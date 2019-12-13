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

namespace Kame {

  Renderer* Renderer::_Instance = nullptr;

  Renderer::Renderer() {
    _ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);
    _SdrRenderProgram = GraphicsCore::CreateRenderProgramNc();
    _SdrRootSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    _SceneRenderTarget = GraphicsCore::CreateRenderTargetNc();
  }

  Renderer::~Renderer() {}

  void Renderer::Initialize(Display* targetDisplay) {
    _TargetDIsplay = targetDisplay;
    CreateSceneTexture();
    CreateSceneDepthTexture();
    CreateSdrProgram();
    _SceneRenderTarget->AttachTexture(AttachmentPoint::Color0, _SceneTexture.get());
    _SceneRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, _SceneDepthTexture.get());
  }

  D3D12_RT_FORMAT_ARRAY Renderer::GetRenderTargetFormats() {
    return _SceneRenderTarget->GetRenderTargetFormats();
  }

  DXGI_FORMAT Renderer::GetDepthStencilFormat() {
    return _SceneRenderTarget->GetDepthStencilFormat();
  }

  void Renderer::Render(Game* game) {
    Reference<CommandList> commandList = GraphicsCore::BeginCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    FLOAT clearColor[] = { 1.0f, 0.4f, 0.7f, 1.0f };

    commandList->ClearTexture(_SceneRenderTarget->GetTexture(AttachmentPoint::Color0), clearColor);
    commandList->ClearDepthStencilTexture(_SceneRenderTarget->GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);

    //TODO f.e. layer3d in game->Get3dLayers
    // layer->SetViewport
    // layer3d->RenderTo(_SceneRenderTarget)

    commandList->SetRenderTarget(*_SceneRenderTarget);

    for (Layer* layer : game->GetLayerStack()) {
      D3D12_VIEWPORT vp = _SceneRenderTarget->GetViewport();
      ScreenRectangle sr = layer->GetScreenRectangle();
      //vp.TopLeftX = sr.Left * vp.Width;
      //vp.TopLeftY = sr.Top * vp.Height;
      //vp.Width = vp.Width * sr.Width;
      //vp.Height = vp.Height * sr.Height;
      _ScissorRect.left = sr.Left * vp.Width;
      _ScissorRect.top = sr.Top * vp.Height;
      _ScissorRect.right = _ScissorRect.left + sr.Width * vp.Width;
      _ScissorRect.bottom = _ScissorRect.top + sr.Height * vp.Height;
      commandList->SetViewport(vp);
      commandList->SetScissorRect(_ScissorRect);

      Renderer3D::RenderScene(
        layer->GetActiveCamera(),
        commandList.get(),
        layer->GetScene()
      );
    }


    //game->RenderTo(_SceneRenderTarget);

    // Final Rendering to the Rendertarget of the Window (here with Tonamapping)
    commandList->SetRenderTarget(_TargetDIsplay->GetRenderTarget());
    commandList->SetViewport(_TargetDIsplay->GetRenderTarget().GetViewport());
    commandList->SetRenderProgram(_SdrRenderProgram.get());
    commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->SetGraphicsRootSignature(_SdrRootSignature.get());
    commandList->SetGraphics32BitConstants(0, ToneMapping::GetParameters1());
    commandList->SetShaderResourceViewTexture(1, 0, _SceneRenderTarget->GetTexture(Color0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->Draw(3);

    GraphicsCore::ExecuteCommandList(commandList);

    // Render GUI.
    //OnGUI();

    // Present
    _TargetDIsplay->Present();

  }

  void Renderer::CreateSceneTexture() {
    DXGI_FORMAT hDRFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    // Create an off-screen render target with a single color buffer and a depth buffer.
    auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(hDRFormat, _TargetDIsplay->GetWidth(), _TargetDIsplay->GetHeight());
    colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE colorClearValue;
    colorClearValue.Format = colorDesc.Format;
    colorClearValue.Color[0] = 0.4f;
    colorClearValue.Color[1] = 0.6f;
    colorClearValue.Color[2] = 0.9f;
    colorClearValue.Color[3] = 1.0f;

    _SceneTexture = GraphicsCore::CreateTextureNc(
      colorDesc, &colorClearValue,
      TextureUsage::RenderTarget,
      L"HDR Texture"
    );

  }

  void Renderer::CreateSceneDepthTexture() {
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;
    auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, _TargetDIsplay->GetWidth(), _TargetDIsplay->GetHeight());
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthClearValue;
    depthClearValue.Format = depthDesc.Format;
    depthClearValue.DepthStencil = { 1.0f, 0 };

    _SceneDepthTexture = GraphicsCore::CreateTextureNc(depthDesc, &depthClearValue, TextureUsage::Depth, L"Depth Render Target");

    // Attach the HDR texture to the HDR render target.
    //m_HDRRenderTarget->AttachTexture(AttachmentPoint::Color0, _HDRTexture.get());
    //m_HDRRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, _DepthTexture.get());
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
    _SdrRenderProgram->SetPrimitiveTopologyType1(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _SdrRenderProgram->SetVertexShader1(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _SdrRenderProgram->SetPixelShader1(CD3DX12_SHADER_BYTECODE(ps.Get()));
    _SdrRenderProgram->SetRasterizer(rasterizerDesc);
    _SdrRenderProgram->SetRenderTargetFormats1(_TargetDIsplay->GetRenderTarget().GetRenderTargetFormats());

    _SdrRenderProgram->Create();
  }

}
