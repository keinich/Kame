#include "kmpch.h"
#include "GraphicsCore.h"

#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>

//TODO Remove this:
#include <Kame/Platform/DirectX12/Graphics/CommandListDx12.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h>

#include <Kame/Graphics/RenderTarget.h>
#include <Kame/Platform/DirectX12/Graphics/TextureDx12.h>

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

  Reference<CommandListDx12> GraphicsCore::BeginCommandListDx(D3D12_COMMAND_LIST_TYPE type) {
    auto commandQueue = s_Instance->GetCommandQueue(type);
    auto commandList = commandQueue->GetCommandList();
    return commandList;
  }

  CommandList* GraphicsCore::BeginCommandList(D3D12_COMMAND_LIST_TYPE type) {
    auto commandQueue = s_Instance->GetCommandQueue(type);
    auto commandList = commandQueue->GetCommandList();
    return commandList.get();
  }

  void GraphicsCore::ExecuteCommandList(Reference<CommandListDx12> commandList) {
    s_Instance->GetCommandQueue(commandList->GetCommandListType())->ExecuteCommandList(commandList);
  }

  RenderTarget* GraphicsCore::CreateRenderTarget() {
    return reinterpret_cast<RenderTarget*>(new RenderTargetOf<TextureDx12>());
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

}