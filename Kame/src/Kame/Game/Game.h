#pragma once

#include <vector>

#include "Kame/Events/EventArgs.h"

#include "memory"
#include "string"  

#include <Kame/Core/References.h>
#include <Kame/Game/LayerStack.h>

namespace Kame {

  class Window;
  class Renderer3D;
  class Scene3D;
  class MeshComponent;
  class Camera;
  class RenderProgram;
  class Texture;
  class MaterialInstanceBase;
  class Scene3D;

  class Game : public std::enable_shared_from_this<Game> {
  public:
    /**
     * Create the DirectX demo using the specified window dimensions.
     */
    Game(const std::wstring& name, int width, int height, bool vSync);
    virtual ~Game();

    int GetAbsoluteWidthInPixels() const {
      return _ScreenRectangle.GetAbsoluteWidth();
    }

    int GetAbsoluteHeightInPixels() const {
      return _ScreenRectangle.GetAbsoluteHeight();
    }

    virtual bool Initialize();

    virtual bool LoadContent() = 0;

    virtual void UnloadContent() = 0;

    virtual void Destroy();

    void Render();

    inline Scene3D* GetScene() const { return _Scene3D.get(); }

    virtual Camera* GetActiveCamera() { return nullptr; }

    inline LayerStack& GetLayerStack() { return _LayerStack; }

    inline ScreenRectangle& GetScreenRectangle() { return _ScreenRectangle; }

  protected:
    //friend class Display;
    friend class Window;

    virtual void OnUpdate(UpdateEventArgs& e);

    virtual void OnRender(RenderEventArgs& e);

    virtual void OnKeyPressed(KeyEventArgs& e);

    virtual void OnKeyReleased(KeyEventArgs& e);

    virtual void OnMouseMoved(MouseMotionEventArgs& e);

    virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);

    virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);

    virtual void OnMouseWheel(MouseWheelEventArgs& e);

    virtual void OnResize(ResizeEventArgs& e);

    virtual void OnWindowDestroy();

  protected: // Fields

    std::shared_ptr<Window> _Window;
    NotCopyableReference<Scene3D> _Scene3D;
    LayerStack _LayerStack;

  private: //Fields

    std::wstring m_Name;    
    bool m_vSync;
    ScreenRectangle _ScreenRectangle;

  };

}