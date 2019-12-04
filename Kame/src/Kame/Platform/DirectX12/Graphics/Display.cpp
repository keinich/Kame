#include "kmpch.h"

#include "Display.h"

#include "DX12Core.h"
#include "CommandQueue.h"
#include "CommandListDx12.h"
#include "Game.h"
#include "GUI.h"
#include "Kame/Graphics/RenderTarget.h"
#include "ResourceStateTracker.h"
#include "TextureDx12.h"
#include <Kame/Core/DebugUtilities.h>

namespace Kame {

  Display::Display(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync) :
    m_ClientWidth(clientWidth)
    , m_ClientHeight(clientHeight)
    , m_VSync(vSync)
    , m_FenceValues{ 0 }
    , m_FrameValues{ 0 } {

    m_RenderTarget.reset(GraphicsCore::CreateRenderTarget());

  }

  void Display::Initialize(HWND hWnd) {

    DX12Core& app = DX12Core::Get();

    m_IsTearingSupported = app.IsTearingSupported();

    for (int i = 0; i < BufferCount; ++i) {
      m_BackBufferTextures[i].SetName(L"Backbuffer[" + std::to_wstring(i) + L"]");
    }

    m_dxgiSwapChain = CreateSwapChain(hWnd);
    UpdateRenderTargetViews();
  }

  Display::~Display() {}

  void Display::Initialize() {
    //m_GUI.Initialize( shared_from_this() );
  }

  int Display::GetClientWidth() const {
    return m_ClientWidth;
  }

  int Display::GetClientHeight() const {
    return m_ClientHeight;
  }

  bool Display::IsVSync() const {
    return m_VSync;
  }

  void Display::SetVSync(bool vSync) {
    m_VSync = vSync;
  }

  void Display::ToggleVSync() {
    SetVSync(!m_VSync);
  }

  void Display::OnResize(ResizeEventArgs& e) {
    // Update the client size.
    if (m_ClientWidth != e.Width || m_ClientHeight != e.Height) {
      m_ClientWidth = std::max(1, e.Width);
      m_ClientHeight = std::max(1, e.Height);

      DX12Core::Get().Flush();

      // Release all references to back buffer textures.
      m_RenderTarget->AttachTexture(Color0, &TextureDx12());
      for (int i = 0; i < BufferCount; ++i) {
        ResourceStateTracker::RemoveGlobalResourceState(m_BackBufferTextures[i].GetD3D12Resource().Get());
        m_BackBufferTextures[i].Reset();
      }

      DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
      ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
      ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(BufferCount, m_ClientWidth,
        m_ClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

      m_CurrentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

      UpdateRenderTargetViews();
    }

  }

  Microsoft::WRL::ComPtr<IDXGISwapChain4> Display::CreateSwapChain(HWND hWnd) {
    DX12Core& app = DX12Core::Get();

    ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_ClientWidth;
    swapChainDesc.Height = m_ClientHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 1, 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = BufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = m_IsTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    ID3D12CommandQueue* pCommandQueue = app.GetCommandQueue()->GetD3D12CommandQueue().Get();

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
      pCommandQueue,
      hWnd,
      &swapChainDesc,
      nullptr,
      nullptr,
      &swapChain1));

    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

    m_CurrentBackBufferIndex = dxgiSwapChain4->GetCurrentBackBufferIndex();

    return dxgiSwapChain4;
  }

  void Display::UpdateRenderTargetViews() {
    for (int i = 0; i < BufferCount; ++i) {
      ComPtr<ID3D12Resource> backBuffer;
      ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

      ResourceStateTracker::AddGlobalResourceState(backBuffer.Get(), D3D12_RESOURCE_STATE_COMMON);

      m_BackBufferTextures[i].SetD3D12Resource(backBuffer);
      m_BackBufferTextures[i].CreateViews();
    }
  }

  const RenderTarget& Display::GetRenderTarget() {
    m_RenderTarget->AttachTexture(AttachmentPoint::Color0, &(m_BackBufferTextures[m_CurrentBackBufferIndex]));
    return *m_RenderTarget;
  }

  UINT Display::Present(const TextureDx12& texture) {
    auto commandQueue = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    auto commandList = commandQueue->GetCommandList();

    auto& backBuffer = m_BackBufferTextures[m_CurrentBackBufferIndex];

    if (texture.IsValid()) {
      if (texture.GetD3D12ResourceDesc().SampleDesc.Count > 1) {
        commandList->ResolveSubresource(backBuffer, texture);
      }
      else {
        commandList->CopyResource(backBuffer, texture);
      }
    }

    //RenderTarget renderTarget;
    //renderTarget.AttachTexture(AttachmentPoint::Color0, backBuffer);

    //m_GUI.Render( commandList, renderTarget );

    commandList->TransitionBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
    commandQueue->ExecuteCommandList(commandList);

    UINT syncInterval = m_VSync ? 1 : 0;
    UINT presentFlags = m_IsTearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(m_dxgiSwapChain->Present(syncInterval, presentFlags));

    m_FenceValues[m_CurrentBackBufferIndex] = commandQueue->Signal();
    m_FrameValues[m_CurrentBackBufferIndex] = DX12Core::GetFrameCount();

    m_CurrentBackBufferIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

    commandQueue->WaitForFenceValue(m_FenceValues[m_CurrentBackBufferIndex]);

    DX12Core::Get().ReleaseStaleDescriptors(m_FrameValues[m_CurrentBackBufferIndex]);

    return m_CurrentBackBufferIndex;
  }

}