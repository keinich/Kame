#pragma once



namespace Kame {

  class CommandListDx12;
  class CommandList;
  class CommandQueue;

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

    static std::shared_ptr<CommandListDx12> BeginCommandListDx(D3D12_COMMAND_LIST_TYPE type);
    static CommandList* BeginCommandList(D3D12_COMMAND_LIST_TYPE type);
    static void ExecuteCommandList(std::shared_ptr<CommandListDx12> commandList);

    static inline RenderApi GetApi() { return s_RenderApi; }

  protected: // Methods

    virtual std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const = 0;

  protected: // Fields

  private:

    static GraphicsCore* s_Instance;
    static RenderApi s_RenderApi;

  };

}