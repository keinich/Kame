#pragma once

namespace Kame {

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

  private:

    static RenderApi s_RenderApi;

  };

}