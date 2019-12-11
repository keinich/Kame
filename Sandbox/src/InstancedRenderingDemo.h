#pragma once

//#include <Kame.h>

//TODO include the correct headers in Kame.h
#include <Kame/Game/Objects/MeshComponent.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Game/Game.h>
#include <Kame/Graphics/Camera.h>
#include <Kame/Game/CameraController.h>

class InstancedRenderingDemo : public Kame::Game {

public:

  InstancedRenderingDemo(const std::wstring& name, int width, int height, bool vSync);
  ~InstancedRenderingDemo();

  virtual Kame::Camera* GetActiveCamera() override;

  virtual bool LoadContent() override;
  virtual void UnloadContent() override;

  virtual void OnRender(Kame::RenderEventArgs& e);
  virtual void OnUpdate(Kame::UpdateEventArgs& e);

private: //Methods

private: //Fields

  Kame::Mesh* _SphereMesh;
  Kame::Mesh* _TorusMesh;
  std::vector<Kame::Reference<Kame::MeshComponent>> _Meshes;
  Kame::Texture* _KameDefaultTexture;
  Kame::Camera _Camera;
  Kame::CameraController _CameraController;

  Kame::Reference<Kame::MaterialInstanceBase> _MaterialInstance;

  Kame::NotCopyableReference<Kame::RenderProgramSignature> _RenderProgramSignature;
  Kame::NotCopyableReference<Kame::RenderProgram> _RenderProgram;

  struct alignas(16) CameraData {
    Kame::Math::Vector4 _InitialCamPos;
    Kame::Math::Vector4 _InitialCamRot;
    float _InitialFov;
  };
  CameraData* _pAlignedCameraData;

};