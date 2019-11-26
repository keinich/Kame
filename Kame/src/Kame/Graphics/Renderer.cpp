#include "kmpch.h"
#include "Renderer.h"

#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>

//TODO Remove this:
#include <Kame/Platform/DirectX12/Graphics/CommandList.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h>

namespace Kame {

  RenderApi Renderer::s_RenderApi = RenderApi::DirectX12;
  Renderer* Renderer::s_Instance = nullptr;

  void Kame::Renderer::Create() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Create();
      s_Instance = &DX12Core::Get();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void Kame::Renderer::Initialize() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Initialize();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void Kame::Renderer::Destroy() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Destroy();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  void Renderer::Flush() {
  
    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Flush();
      return;
    }
    KM_ASSERT(false, "Unknown RenderApi");

  }

  std::shared_ptr<CommandList> Renderer::BeginCommandList(D3D12_COMMAND_LIST_TYPE type) {
    auto commandQueue = DX12Core::Get().GetCommandQueue(type);
    auto commandList = commandQueue->GetCommandList();
    return commandList;
  }

  void Renderer::ExecuteCommandList(std::shared_ptr<CommandList> commandList) {
    DX12Core::Get().GetCommandQueue(commandList->GetCommandListType())->ExecuteCommandList(commandList);
  }

}