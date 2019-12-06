#pragma once

#include <Kame/Core/References.h>

namespace Kame {

  class Texture;
  class RenderProgramSignature;
  class RenderProgram;
  class Camera;
  class Display;
  class Game;
  class RenderTarget;

  class Renderer {

  public:

    //static void Create();
    //static void Destroy();
    //inline static Renderer* Get() { return _Instance; }

    void Initialize(Display* targetWindow);

    void Render(Game* game);

    D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats();
    DXGI_FORMAT GetDepthStencilFormat();

    Renderer();
    virtual ~Renderer();

  private: //Methods

    void CreateSceneTexture();
    void CreateSceneDepthTexture();
    void CreateSdrProgram();

  private: //Fields

    static Renderer* _Instance;

    NotCopyableReference<Texture> _SceneTexture;
    NotCopyableReference<Texture> _SceneDepthTexture;
    NotCopyableReference<RenderTarget> _SceneRenderTarget;

    NotCopyableReference<RenderProgramSignature> _SdrRootSignature;
    NotCopyableReference<RenderProgram> _SdrRenderProgram;

    Display* _TargetDIsplay;

    D3D12_RECT _ScissorRect; 

  };

}