#pragma once

//System
#include <string>
#include <d3d12.h> //TODO abstract away D3D12_RESOURCE_DESC, D3D12_CLEAR_VALUE

//Kame
#include "RenderApi/TextureUsage.h"
#include <Kame/Core/References.h>

namespace Kame {

  class CommandListDx12;
  class CommandList;
  class CommandQueue;

  class RenderTarget;
  class Texture;
  class RenderProgramSignature;
  class RenderProgram;
  class VertexBuffer;
  class IndexBuffer;

  enum class RenderApi {

    None = 0,
    DirectX12 = 1

  };

  class GraphicsCore { //TODO Rename to GraphicsCore or GraphicsEngine

  public:

    static void Create();
    static void Initialize();
    static void Destroy();

    static void Flush();

    static Reference<CommandList> BeginCommandList(D3D12_COMMAND_LIST_TYPE type);
    static void ExecuteCommandList(Reference<CommandList> commandList, bool waitForCompletion = false);

    static RenderTarget* CreateRenderTarget();
    static NotCopyableReference<RenderTarget> CreateRenderTargetNc();

    static NotCopyableReference<VertexBuffer> CreateVertexBuffer();
    static NotCopyableReference<IndexBuffer> CreateIndexBuffer();

    static NotCopyableReference<RenderProgramSignature> CreateRenderProgramSignatureNc();
    static NotCopyableReference<RenderProgram> CreateRenderProgramNc();

    static Texture* GraphicsCore::CreateTexture();

    static Reference<Texture> CreateTexture(
      const D3D12_RESOURCE_DESC& resourceDesc,
      const D3D12_CLEAR_VALUE* clearValue,
      TextureUsage textureUsage,
      const std::wstring& name
    );
    static NotCopyableReference<Texture> CreateTextureNc(
      const D3D12_RESOURCE_DESC& resourceDesc,
      const D3D12_CLEAR_VALUE* clearValue,
      TextureUsage textureUsage,
      const std::wstring& name
    );

    static inline RenderApi GetApi() { return s_RenderApi; }

    static void ReportLiveObjects();

  protected: // Methods

    virtual Reference<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const = 0;

  protected: // Fields

  private:

    static GraphicsCore* s_Instance;
    static RenderApi s_RenderApi;

  };

}