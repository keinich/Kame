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

    void Initialize(Display* targetWindow);

    void Render(Game* game);

    Renderer();
    virtual ~Renderer();

  private: //Methods

    void CreateSdrProgram();

  private: //Fields

    static Renderer* _Instance;

    NotCopyableReference<RenderProgramSignature> _SdrRootSignature;
    NotCopyableReference<RenderProgram> _SdrRenderProgram;

    Display* _TargetDIsplay;

    D3D12_RECT _ScissorRect;

  };

}