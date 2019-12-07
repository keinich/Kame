#pragma once

//#include <Kame.h>

//TODO include the correct headers in Kame.h
#include <Kame/Graphics/Mesh.h>
#include <Kame/Game/Game.h>
#include <Kame/Graphics/Camera.h>

class InstancedRenderingDemo : public Kame::Game {

public:

  InstancedRenderingDemo(const std::wstring& name, int width, int height, bool vSync);
  ~InstancedRenderingDemo();

  virtual std::vector<Kame::Mesh*>& GetMeshes() override;
  virtual Kame::RenderProgram* GetRenderProgram() override;
  virtual Kame::Texture* GetTexture() override; 
  virtual Kame::Camera* GetActiveCamera() override;
  virtual Kame::MaterialInstanceBase* GetMaterial() override;

  virtual bool LoadContent() override;
  virtual void UnloadContent() override;

  virtual void OnRender(Kame::RenderEventArgs& e);

private: //Methods

  void CreateProgram();

private: //Fields

  Kame::Mesh* _SphereMesh;
  std::vector<Kame::Mesh*> _Meshes;
  Kame::Texture* _KameDefaultTexture;  
  Kame::Camera _Camera;

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