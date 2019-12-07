#include "kmpch.h"
#include "Material.h"
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame\Graphics\Mesh.h>
#include <Kame/Core/DebugUtilities.h>
#include <Kame\Graphics\Renderer3D.h>
#include <Kame/Graphics/GraphicsCore.h>

namespace Kame {

  const BaseMaterialParameters BaseMaterialParameters::Red =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Green =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.1f, 0.0f, 1.0f },
      { 0.0f, 1.0f, 0.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Blue =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.1f, 1.0f },
      { 0.0f, 0.0f, 1.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Cyan =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.1f, 0.1f, 1.0f },
      { 0.0f, 1.0f, 1.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Magenta =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1f, 0.0f, 0.1f, 1.0f },
      { 1.0f, 0.0f, 1.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Yellow =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.1f, 0.1f, 1.0f },
      { 0.0f, 1.0f, 1.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::White =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1f, 0.1f, 0.1f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Black =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 1.0f, 1.0f, 1.0f, 1.0f },
      128.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::Emerald =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0215f, 0.1745f, 0.0215f, 1.0f },
      { 0.07568f, 0.61424f, 0.07568f, 1.0f },
      { 0.633f, 0.727811f, 0.633f, 1.0f },
      76.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Jade =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.135f, 0.2225f, 0.1575f, 1.0f },
      { 0.54f, 0.89f, 0.63f, 1.0f },
      { 0.316228f, 0.316228f, 0.316228f, 1.0f },
      12.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Obsidian =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.05375f, 0.05f, 0.06625f, 1.0f },
      { 0.18275f, 0.17f, 0.22525f, 1.0f },
      { 0.332741f, 0.328634f, 0.346435f, 1.0f },
      38.4f
  };

  const BaseMaterialParameters BaseMaterialParameters::Pearl =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.25f, 0.20725f, 0.20725f, 1.0f },
      { 1.0f, 0.829f, 0.829f, 1.0f },
      { 0.296648f, 0.296648f, 0.296648f, 1.0f },
      11.264f
  };

  const BaseMaterialParameters BaseMaterialParameters::Ruby = {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1745f, 0.01175f, 0.01175f, 1.0f },
      { 0.61424f, 0.04136f, 0.04136f, 1.0f },
      { 0.727811f, 0.626959f, 0.626959f, 1.0f },
      76.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Turquoise =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1f, 0.18725f, 0.1745f, 1.0f },
      { 0.396f, 0.74151f, 0.69102f, 1.0f },
      { 0.297254f, 0.30829f, 0.306678f, 1.0f },
      12.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Brass =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.329412f, 0.223529f, 0.027451f, 1.0f },
      { 0.780392f, 0.568627f, 0.113725f, 1.0f },
      { 0.992157f, 0.941176f, 0.807843f, 1.0f },
      27.9f
  };

  const BaseMaterialParameters BaseMaterialParameters::Bronze =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.2125f, 0.1275f, 0.054f, 1.0f },
      { 0.714f, 0.4284f, 0.18144f, 1.0f },
      { 0.393548f, 0.271906f, 0.166721f, 1.0f },
      25.6f
  };

  const BaseMaterialParameters BaseMaterialParameters::Chrome =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.25f, 0.25f, 0.25f, 1.0f },
      { 0.4f, 0.4f, 0.4f, 1.0f },
      { 0.774597f, 0.774597f, 0.774597f, 1.0f },
      76.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Copper =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.19125f, 0.0735f, 0.0225f, 1.0f },
      { 0.7038f, 0.27048f, 0.0828f, 1.0f },
      { 0.256777f, 0.137622f, 0.086014f, 1.0f },
      12.8f
  };

  const BaseMaterialParameters BaseMaterialParameters::Gold =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.24725f, 0.1995f, 0.0745f, 1.0f },
      { 0.75164f, 0.60648f, 0.22648f, 1.0f },
      { 0.628281f, 0.555802f, 0.366065f, 1.0f },
      51.2f
  };

  const BaseMaterialParameters BaseMaterialParameters::Silver =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.19225f, 0.19225f, 0.19225f, 1.0f },
      { 0.50754f, 0.50754f, 0.50754f, 1.0f },
      { 0.508273f, 0.508273f, 0.508273f, 1.0f },
      51.2f
  };

  const BaseMaterialParameters BaseMaterialParameters::BlackPlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.01f, 0.01f, 0.01f, 1.0f },
      { 0.5f, 0.5f, 0.5f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::CyanPlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.1f, 0.06f, 1.0f },
      { 0.0f, 0.50980392f, 0.50980392f, 1.0f },
      { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::GreenPlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.1f, 0.35f, 0.1f, 1.0f },
      { 0.45f, 0.55f, 0.45f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::RedPlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.5f, 0.0f, 0.0f, 1.0f },
      { 0.7f, 0.6f, 0.6f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::WhitePlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.55f, 0.55f, 0.55f, 1.0f },
      { 0.7f, 0.7f, 0.7f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::YellowPlastic =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.5f, 0.5f, 0.0f, 1.0f },
      { 0.6f, 0.6f, 0.5f, 1.0f },
      32.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::BlackRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.02f, 0.02f, 0.02f, 1.0f },
      { 0.01f, 0.01f, 0.01f, 1.0f },
      { 0.4f, 0.4f, 0.4f, 1.0f },
      10.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::CyanRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.05f, 0.05f, 1.0f },
      { 0.4f, 0.5f, 0.5f, 1.0f },
      { 0.04f, 0.7f, 0.7f, 1.0f },
      10.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::GreenRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.05f, 0.0f, 1.0f },
      { 0.4f, 0.5f, 0.4f, 1.0f },
      { 0.04f, 0.7f, 0.04f, 1.0f },
      10.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::RedRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.05f, 0.0f, 0.0f, 1.0f },
      { 0.5f, 0.4f, 0.4f, 1.0f },
      { 0.7f, 0.04f, 0.04f, 1.0f },
      10.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::WhiteRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.05f, 0.05f, 0.05f, 1.0f },
      { 0.5f, 0.5f, 0.5f, 1.0f },
      { 0.7f, 0.7f, 0.7f, 1.0f },
      10.0f
  };

  const BaseMaterialParameters BaseMaterialParameters::YellowRubber =
  {
      { 0.0f, 0.0f, 0.0f, 1.0f },
      { 0.05f, 0.05f, 0.0f, 1.0f },
      { 0.5f, 0.5f, 0.4f, 1.0f },
      { 0.7f, 0.7f, 0.04f, 1.0f },
      10.0f
  };

  enum DefaultMaterialRootParameters {
    MatricesCB1,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCB1,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCB1,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    PointLights1,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLights1,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    Textures1,           // Texture2D DiffuseTexture : register( t2 );
    NumRootParameters1
  };

  void DefaultMaterial::ApplyParameters(CommandList* commandList, DefaultMaterialParameters& params) {
    commandList->SetGraphicsDynamicConstantBuffer(DefaultMaterialRootParameters::MaterialCB1, params.BaseParams);
    commandList->SetShaderResourceViewTexture(DefaultMaterialRootParameters::Textures1, 0, params.DiffuseTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
  }

  void DefaultMaterial::CreateProgram() {

    //TODO Have Base RootSignature and Base RootParameters
    //Here should only be declared the extensions of thos Bases

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

    CD3DX12_ROOT_PARAMETER1 rootParameters[Kame::DefaultMaterialRootParameters::NumRootParameters1];
    rootParameters[Kame::DefaultMaterialRootParameters::MatricesCB1].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[Kame::DefaultMaterialRootParameters::MaterialCB1].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[Kame::DefaultMaterialRootParameters::LightPropertiesCB1].InitAsConstants(sizeof(Kame::LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[Kame::DefaultMaterialRootParameters::PointLights1].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[Kame::DefaultMaterialRootParameters::SpotLights1].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[Kame::DefaultMaterialRootParameters::Textures1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
    CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(Kame::DefaultMaterialRootParameters::NumRootParameters1, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

    //m_HDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
    _ProgramSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    _Program->SetRootSignature(_ProgramSignature.get());
    _Program->SetInputLayout1(Kame::VertexPositionNormalTexture::InputElementCount, Kame::VertexPositionNormalTexture::InputElements);
    _Program->SetPrimitiveTopologyType1(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _Program->SetVertexShader1(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _Program->SetPixelShader1(CD3DX12_SHADER_BYTECODE(ps.Get()));
    D3D12_RT_FORMAT_ARRAY rtFormat = {}; //TODO Woher kriegen wenn nicht stehlen?
    rtFormat.NumRenderTargets = 1;
    rtFormat.RTFormats[0] =  DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT dsFormat = DXGI_FORMAT_D32_FLOAT;
    _Program->SetRenderTargetFormats1(
      rtFormat,
      dsFormat
    );

    _Program->Create();
  }

  MaterialBase::MaterialBase() {
    _ProgramSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    _Program = GraphicsCore::CreateRenderProgramNc();
  }

  MaterialBase::~MaterialBase() {}

}