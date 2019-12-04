#include "kmpch.h"

//System
#include <dxgidebug.h>

//Kame
#include <Kame/Core/DebugUtilities.h>
#include "GraphicsCore.h"

#include <Kame/Graphics/RenderTarget.h>
#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Platform/DirectX12/Graphics/CommandListDx12.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h>
#include <Kame/Platform/DirectX12/Graphics/TextureDx12.h>
#include <Kame/Platform/DirectX12/Graphics/RootSignatureDx12.h>
#include "Kame/Platform/DirectX12/Graphics/VertexBufferDx12.h"
#include "Kame/Platform/DirectX12/Graphics/IndexBufferDx12.h"

namespace Kame {

  RenderApi GraphicsCore::s_RenderApi = RenderApi::DirectX12;
  GraphicsCore* GraphicsCore::s_Instance = nullptr;

  void Kame::GraphicsCore::Create() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Create();
      s_Instance = &DX12Core::Get();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void Kame::GraphicsCore::Initialize() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Initialize();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void Kame::GraphicsCore::Destroy() {



    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Destroy();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void GraphicsCore::Flush() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Flush();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  Reference<CommandList> GraphicsCore::BeginCommandList(D3D12_COMMAND_LIST_TYPE type) {
    auto commandQueue = s_Instance->GetCommandQueue(type);
    auto commandList = commandQueue->GetCommandList();
    return commandList;
  }

  void GraphicsCore::ExecuteCommandList(Reference<CommandList> commandList, bool waitForCompletion) {
    uint64_t fenceValue =  s_Instance->GetCommandQueue(commandList->GetType())->ExecuteCommandList(commandList);
    if (waitForCompletion) {
      s_Instance->GetCommandQueue(commandList->GetType())->WaitForFenceValue(fenceValue);
    }    
  }

  RenderTarget* GraphicsCore::CreateRenderTarget() {
    return reinterpret_cast<RenderTarget*>(new RenderTargetOf<TextureDx12>());
  }

  NotCopyableReference<VertexBuffer> GraphicsCore::CreateVertexBuffer() {
    return CreateNotCopyableReference<VertexBufferDx12>();
  }

  NotCopyableReference<IndexBuffer> GraphicsCore::CreateIndexBuffer() {
    return CreateNotCopyableReference<IndexBufferDx12>();
  }

  NotCopyableReference<RenderProgramSignature> GraphicsCore::CreateRenderProgramSignatureNc() {
    return CreateNotCopyableReference<RootSignatureDx12>();
  }

  Texture* GraphicsCore::CreateTexture() {
    return new TextureDx12();
  }

  Reference<Texture> GraphicsCore::CreateTexture(
    const D3D12_RESOURCE_DESC& resourceDesc,
    const D3D12_CLEAR_VALUE* clearValue,
    TextureUsage textureUsage,
    const std::wstring& name
  ) {
    Reference<Texture> ret = CreateReference<TextureDx12>(resourceDesc, clearValue, textureUsage, name);
    return ret;
    //return reinterpret_cast<Texture*>(new TextureDx12(resourceDesc, clearValue, textureUsage, name));
  }

  void GraphicsCore::ReportLiveObjects() {
    IDXGIDebug1* dxgiDebug;
    DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

    dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL);
    dxgiDebug->Release();
  }
}