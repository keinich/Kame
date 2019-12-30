#include "kmpch.h"
#include "Game.h"

#include "Kame/Application/Application.h"
#include "Kame/Application/Window.h"
#include <Kame/Graphics/Renderer3D.h>
#include <Kame/Graphics/Renderer.h>
#include <Kame/Graphics/Mesh.h>
#include <Kame/Graphics/Scene3D.h>

namespace Kame {

  Game::Game(const std::wstring& name, int width, int height, bool vSync)
    : m_Name(name)
    , m_vSync(vSync)
    , _ScreenRectangle(0, 0, width, height) {
    _Scene3D = CreateNotCopyableReference<Scene3D>();
  }

  Game::~Game() {
    assert(!_Window && "Use Game::Destroy() before destruction.");
  }

  bool Game::Initialize() {
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport()) {
      MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
      return false;
    }

    _Window = Application::Get().CreateRenderWindow(
      m_Name,_ScreenRectangle.GetAbsoluteWidth(), 
      _ScreenRectangle.GetAbsoluteHeight(), 
      m_vSync
    );
    _Window->RegisterCallbacks(shared_from_this());
    _Window->Show();

    return true;
  }

  void Game::Destroy() {
    Application::Get().DestroyWindow(_Window);
    _Window.reset();
  }

  void Game::Render() {
    //_Renderer3D->Render(_Meshes);
  }

  void Game::OnUpdate(UpdateEventArgs& e) {}

  void Game::OnRender(RenderEventArgs& e) {

  }

  void Game::OnKeyPressed(KeyEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnKeyReleased(KeyEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnMouseMoved(class MouseMotionEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnMouseButtonPressed(MouseButtonEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnMouseButtonReleased(MouseButtonEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnMouseWheel(MouseWheelEventArgs& e) {
    // By default, do nothing.
  }

  void Game::OnResize(ResizeEventArgs& e) {
    _ScreenRectangle.SetAbsoluteWidth(e.Width);
    _ScreenRectangle.SetAbsoluteHeight(e.Height);
    for (Layer* layer : _LayerStack) {
      layer->OnResize(e);
    }
  }

  void Game::OnWindowDestroy() {
    // If the Window which we are registered to is 
    // destroyed, then any resources which are associated 
    // to the window must be released.
    UnloadContent();
  }

}