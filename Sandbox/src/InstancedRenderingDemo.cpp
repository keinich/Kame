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

using namespace Microsoft::WRL;
using namespace DirectX;

InstancedRenderingDemo::InstancedRenderingDemo(const std::wstring& name, int width, int height, bool vSync)
  : Game(name, width, height, vSync) {

  Kame::Math::Vector4 cameraPos1(0, 5, -20, 1);
  XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
  XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
  XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

  _Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
  _Camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

  _pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

  _pAlignedCameraData->_InitialCamPos = _Camera.get_Translation();
  _pAlignedCameraData->_InitialCamRot = _Camera.get_Rotation();
  _pAlignedCameraData->_InitialFov = _Camera.get_FoV();

  _RenderProgram = Kame::GraphicsCore::CreateRenderProgramNc();
  _RenderProgramSignature = Kame::GraphicsCore::CreateRenderProgramSignatureNc();
}

InstancedRenderingDemo::~InstancedRenderingDemo() {
  _aligned_free(_pAlignedCameraData);
}

std::vector<Kame::Mesh*>& InstancedRenderingDemo::GetMeshes() {
  return _Meshes;
}

Kame::RenderProgram* InstancedRenderingDemo::GetRenderProgram() {
  return _RenderProgram.get();
}

Kame::Texture* InstancedRenderingDemo::GetTexture() {
  return _KameDefaultTexture;
}

Kame::Camera* InstancedRenderingDemo::GetActiveCamera() {
  return &_Camera;
}

bool InstancedRenderingDemo::LoadContent() {
  _SphereMesh = Kame::MeshFactory::GetSphere();
  _KameDefaultTexture = Kame::TextureManager::GetTexture(L"Assets/Textures/DefaultWhite.bmp");

  _Meshes.push_back(_SphereMesh);

  CreateProgram();

  return true;
}

void InstancedRenderingDemo::UnloadContent() {}

void InstancedRenderingDemo::OnRender(Kame::RenderEventArgs& e) {

  m_pWindow->GetDisplay().GetRenderer()->Render(this);
  //Kame::Renderer::Get()->Render(this);

}

void InstancedRenderingDemo::CreateProgram() {
  // Load the HDR shaders.
  ComPtr<ID3DBlob> vs;
  ComPtr<ID3DBlob> ps;
  ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDR_VS.cso", &vs));
  ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDR_PS.cso", &ps));

  // Allow input layout and deny unnecessary access to certain pipeline stages.
  D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

  CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

  CD3DX12_ROOT_PARAMETER1 rootParameters[Kame::RootParameters::NumRootParameters];
  rootParameters[Kame::RootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
  rootParameters[Kame::RootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
  rootParameters[Kame::RootParameters::LightPropertiesCB].InitAsConstants(sizeof(Kame::LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
  rootParameters[Kame::RootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
  rootParameters[Kame::RootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
  rootParameters[Kame::RootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

  CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
  CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

  CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
  rootSignatureDescription.Init_1_1(Kame::RootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

  //m_HDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
  _RenderProgramSignature->SetDescription(rootSignatureDescription.Desc_1_1);

  _RenderProgram->SetRootSignature(_RenderProgramSignature.get());
  _RenderProgram->SetInputLayout1(Kame::VertexPositionNormalTexture::InputElementCount, Kame::VertexPositionNormalTexture::InputElements);
  _RenderProgram->SetPrimitiveTopologyType1(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
  _RenderProgram->SetVertexShader1(CD3DX12_SHADER_BYTECODE(vs.Get()));
  _RenderProgram->SetPixelShader1(CD3DX12_SHADER_BYTECODE(ps.Get()));
  _RenderProgram->SetRenderTargetFormats1(
    m_pWindow->GetDisplay().GetRenderer()->GetRenderTargetFormats(),
    m_pWindow->GetDisplay().GetRenderer()->GetDepthStencilFormat()
  );

  _RenderProgram->Create();
}
