#pragma once

#include <vector>

#include "Kame/Events/EventArgs.h"

#include "memory"
#include "string"  

#include <Kame/Core/References.h>

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

    int GetClientWidth() const {
      return m_Width;
    }

    int GetClientHeight() const {
      return m_Height;
    }

    virtual bool Initialize();

    virtual bool LoadContent() = 0;

    virtual void UnloadContent() = 0;

    virtual void Destroy();

    void Render();

    inline Scene3D* GetScene() const { return _Scene3D.get(); }

    virtual Camera* GetActiveCamera() { return nullptr; }

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

    std::shared_ptr<Window> m_pWindow;

    NotCopyableReference<Scene3D> _Scene3D;

  private: //Fields

    std::wstring m_Name;
    int m_Width;
    int m_Height;
    bool m_vSync;


  };

}