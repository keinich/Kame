#include "InstancedRenderingDemo.h"

#include <d3dcompiler.h>

#include <Kame/Graphics/MeshFactory.h>
#include <Kame/Graphics/TextureManager.h>
#include <Kame/Core/DebugUtilities.h>
#include <Kame\Platform\DirectX12\Graphics\d3dx12.h>
#include <Kame\Graphics\Renderer3D.h>
#include <Kame\Graphics\Renderer.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/GraphicsCore.h>
#include <Kame/Application/Window.h>
#include <Kame/Graphics/RenderTarget.h>
#include <Kame/Graphics/Material.h>
#include <Kame\Graphics\MaterialManager.h>
#include <Kame/Graphics/Scene3D.h>
#include <Kame\Graphics\Light.h>

using namespace Microsoft::WRL;
using namespace DirectX;

InstancedRenderingDemo::InstancedRenderingDemo(const std::wstring& name, int width, int height, bool vSync)
  : Game(name, width, height, vSync),
  _CameraController(&_Camera) {

  Kame::Math::Vector4 cameraPos1(0, 5, -20, 1);
  XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
  XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
  XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

  _Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
  _Camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

  //_CameraController = Kame::CameraController(&_Camera);

  _pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

  _pAlignedCameraData->_InitialCamPos = _Camera.get_Translation();
  _pAlignedCameraData->_InitialCamRot = _Camera.get_Rotation();
  _pAlignedCameraData->_InitialFov = _Camera.get_FoV();

  _RenderProgram = Kame::GraphicsCore::CreateRenderProgramNc();
  _RenderProgramSignature = Kame::GraphicsCore::CreateRenderProgramSignatureNc();


  _LayerStack.PushLayer(new MinimapLayer(this));
  _LayerStack.PushLayer(new MainLayer(this));
}

InstancedRenderingDemo::~InstancedRenderingDemo() {
  _aligned_free(_pAlignedCameraData);
}

Kame::Camera* InstancedRenderingDemo::GetActiveCamera() {
  return &_Camera;
}

bool InstancedRenderingDemo::LoadContent() {
  _SphereMesh = Kame::MeshFactory::GetSphere();
  _TorusMesh = Kame::MeshFactory::GetTorus();
  //_KameDefaultTexture = Kame::TextureManager::GetTexture(L"Assets/Textures/Directx9.png");

  //CreateProgram();

  //Kame::DefaultMaterial* material = Kame::MaterialManager::GetMaterial<Kame::DefaultMaterial>();

  //auto matInstance = Kame::MaterialInstance<Kame::DefaultMaterialParameters>::CreateFromMaterial(material);
  auto matInstance = Kame::MaterialInstance<Kame::DefaultMaterialParameters>::CreateFromMaterial1<Kame::DefaultMaterial>();
  //matInstance->GetParameters().DiffuseTexture = _KameDefaultTexture;
  matInstance->GetParameters().DiffuseTexture = Kame::TextureManager::GetTexture(L"Assets/Textures/Katze.jpg");
  matInstance->GetParameters().BaseParams.Ambient = Kame::Math::Float4(0.1f, 0.1f, 0.1f, 0.1f);
  matInstance->GetParameters().BaseParams.Diffuse = Kame::Math::Float4(0.9f, 0.9f, 0.9f, 0.9f);
  //matInstance->GetParameters().BaseParams. = Kame::Math::Float4(0.9f, 0.9f, 0.9f, 0.9f);
  _MaterialInstance = matInstance;






  for (int m = 0; m < 10; ++m) {
    for (int n = 0; n < 10; ++n) {

      auto meshComponent = Kame::CreateReference<Kame::MeshComponent>();
      meshComponent->SetMesh(_SphereMesh);
      meshComponent->SetMaterialInstance(_MaterialInstance.get());
      _Meshes.push_back(meshComponent);
      XMMATRIX translationMatrix = XMMatrixTranslation(m * 4.0f, 4.0f, n * 4.0f);
      XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
      XMMATRIX scaleMatrix = XMMatrixScaling(2.0f, 2.0f, 2.0f);
      XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
      meshComponent->SetWorldMatirx(worldMatrix);
      _Scene3D->AddMeshComponent(meshComponent);

    }
  }

  auto meshComponent2 = Kame::CreateReference<Kame::MeshComponent>();
  meshComponent2->SetMesh(_SphereMesh);
  meshComponent2->SetMaterialInstance(_MaterialInstance.get());
  _Meshes.push_back(meshComponent2);
  XMMATRIX translationMatrix2 = XMMatrixTranslation(14.0f, 4.0f, 4.0f);
  XMMATRIX rotationMatrix2 = XMMatrixRotationY(XMConvertToRadians(45.0f));
  XMMATRIX scaleMatrix2 = XMMatrixScaling(4.0f, 4.0f, 4.0f);
  XMMATRIX worldMatrix2 = scaleMatrix2 * rotationMatrix2 * translationMatrix2;
  meshComponent2->SetWorldMatirx(worldMatrix2);
  _Scene3D->AddMeshComponent(meshComponent2);

  auto meshComponent3 = Kame::CreateReference<Kame::MeshComponent>();
  meshComponent3->SetMesh(_TorusMesh);
  meshComponent3->SetMaterialInstance(_MaterialInstance.get());
  _Meshes.push_back(meshComponent3);
  XMMATRIX translationMatrix3 = XMMatrixTranslation(24.0f, 4.0f, 4.0f);
  XMMATRIX rotationMatrix3 = XMMatrixRotationY(XMConvertToRadians(45.0f));
  XMMATRIX scaleMatrix3 = XMMatrixScaling(8.0f, 8.0f, 8.0f);
  XMMATRIX worldMatrix3 = scaleMatrix3 * rotationMatrix3 * translationMatrix3;
  meshComponent3->SetWorldMatirx(worldMatrix3);
  _Scene3D->AddMeshComponent(meshComponent3);


  for (int i = 0; i < 1; ++i) {
    Kame::PointLight l;

    l.PositionWS = XMFLOAT4(0.f, 0.f, 0.f, 1.f);

    l.Color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    l.Intensity = 1.0f;
    l.Attenuation = 0.0f;

    _Scene3D->AddPointLight(l);
  }

  for (int i = 0; i < 1; ++i) {
    Kame::SpotLight l;

    l.PositionWS = XMFLOAT4(0.f, 17.f, 0.f, 1.f);
    l.DirectionWS = XMFLOAT4(0.f, -1.f, 0.f, 0.f);
    l.Color = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
    l.Intensity = 1.0f;
    l.Attenuation = 0.0f;
    l.SpotAngle = XMConvertToRadians(45.0f);

    _Scene3D->AddSpotLight(l);
  }

  //_Scene3D->CreateSkybox(Kame::TextureManager::GetTexture(L"Assets/Textures/glacier.hdr"));
  _Scene3D->CreateSkybox(Kame::TextureManager::GetTexture(L"Assets/Textures/Space.jpg"));


  return true;
}

void InstancedRenderingDemo::UnloadContent() {}

void InstancedRenderingDemo::OnRender(Kame::RenderEventArgs& e) {

  m_pWindow->GetDisplay().GetRenderer()->Render(this);
  //Kame::Renderer::Get()->Render(this);

}

void InstancedRenderingDemo::OnUpdate(Kame::UpdateEventArgs& e) {
  _CameraController.Update(e.ElapsedTime);
}

void InstancedRenderingDemo::OnResize(Kame::ResizeEventArgs& e) {
  Game::OnResize(e);
  float fov = _Camera.get_FoV();
  float aspectRatio = GetClientWidth() / (float)GetClientHeight();
  _Camera.set_Projection(fov, aspectRatio, 0.1f, 100.0f);
}

MainLayer::MainLayer(Kame::Game* game, const std::string& name) :
  Layer(game->GetClientWidth(), game->GetClientHeight(), name) {
  _Game = game;
}

MainLayer::~MainLayer() {}

Kame::Camera* MainLayer::GetActiveCamera() {
  return _Game->GetActiveCamera();
}

Kame::Scene3D* MainLayer::GetScene() {
  return _Game->GetScene();
}

MinimapLayer::MinimapLayer(Kame::Game* game, const std::string& name) :
  Layer(game->GetClientWidth() / 5.0f, game->GetClientHeight() / 5.0f, name) {
  _Game = game;
  _ScreenRectangle.Left = 0.5f;
  _ScreenRectangle.Width = 0.1f;
  _ScreenRectangle.Top = 0.5f;
  _ScreenRectangle.Height = 0.1f;

  Kame::Math::Vector4 cameraPos1(0, 5, -20, 1);
  XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
  XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
  XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

  _Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
  _Camera.set_Projection(45.0f, 100.f / 100.f, 0.1f, 100.0f);
}

MinimapLayer::~MinimapLayer() {}

Kame::Camera* MinimapLayer::GetActiveCamera() {
  return &_Camera;
  //return _Game->GetActiveCamera();
}

Kame::Scene3D* MinimapLayer::GetScene() {
  return _Game->GetScene();
}
