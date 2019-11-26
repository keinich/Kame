#pragma once



namespace Kame {

  class CommandList;

  enum class RenderApi {

    None = 0,
    DirectX12 = 1

  };

  class Renderer {

  public:

    static void Create();
    static void Initialize();
    static void Destroy();

    static void Flush();

    static std::shared_ptr<CommandList> BeginCommandList(D3D12_COMMAND_LIST_TYPE type);
    static void ExecuteCommandList(std::shared_ptr<CommandList> commandList);
  protected:

    static Renderer* s_Instance;

  private:

    static RenderApi s_RenderApi;

  };

}