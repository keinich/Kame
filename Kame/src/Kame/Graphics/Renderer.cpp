#include "kmpch.h"
#include "Renderer.h"

#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>

namespace Kame {

  RenderApi Renderer::s_RenderApi = RenderApi::DirectX12;

  void Kame::Renderer::Create() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Create();
      return;
    }
    assert(false, "Unknown RenderApi");

  }

  void Kame::Renderer::Initialize() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Initialize();
      return;
    }
    assert(false, "Unknown RenderApi");

  }

  void Kame::Renderer::Destroy() {

    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Destroy();
      return;
    }
    assert(false, "Unknown RenderApi");

  }

  void Renderer::Flush() {
  
    switch (s_RenderApi) {
    case RenderApi::DirectX12:
      DX12Core::Get().Flush();
      return;
    }
    assert(false, "Unknown RenderApi");

  }

}