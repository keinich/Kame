//#include <Kame.h>

#include "kmpch.h"

#include "Tutorial4.h"
#include <Kame/Core/DebugUtilities.h>

#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Application/Application.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h>
#include <Kame/Platform/DirectX12/Graphics/CommandListDx12.h>
#include <Kame/Platform/DirectX12/Graphics/Helpers.h>
#include <Kame/Graphics/Light.h>
#include <Kame/Graphics/Material.h>
#include <Kame/Platform/DirectX12/Graphics/Display.h>
#include <Kame/Application/Window.h>
#include <Kame/Platform/DirectX12/Graphics/GraphicsCommon.h>

#include "Kame/Math/Vector4.h"
#include "Kame/Math/VectorMath.h"

#include "Kame/Input/Input.h"

#include "Kame/Graphics/RenderApi/VertexLayout.h"

#include "Kame/Platform/DirectX12/Graphics/RenderProgramDx12.h"

#include"Kame/Graphics/MeshFactory.h"
#include "Kame/Graphics/TextureManager.h"

#include <wrl.h>
using namespace Microsoft::WRL;

#include <Kame/Platform/DirectX12/Graphics/d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <DirectXMath.h>

using namespace DirectX;

#include <algorithm> // For std::min and std::max.
#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

namespace Kame {

  struct Mat {
    XMMATRIX ModelMatrix;
    XMMATRIX ModelViewMatrix;
    XMMATRIX InverseTransposeModelViewMatrix;
    XMMATRIX ModelViewProjectionMatrix;
  };

  struct LightProperties {
    uint32_t NumPointLights;
    uint32_t NumSpotLights;
  };

  enum TonemapMethod : uint32_t {
    TM_Linear,
    TM_Reinhard,
    TM_ReinhardSq,
    TM_ACESFilmic,
  };

  struct TonemapParameters {
    TonemapParameters()
      : TonemapMethod(TM_Reinhard)
      , Exposure(0.0f)
      , MaxLuminance(1.0f)
      , K(1.0f)
      , A(0.22f)
      , B(0.3f)
      , C(0.1f)
      , D(0.2f)
      , E(0.01f)
      , F(0.3f)
      , LinearWhite(11.2f)
      , Gamma(2.2f) {}

    // The method to use to perform tonemapping.
    TonemapMethod TonemapMethod;
    // Exposure should be expressed as a relative exposure value (-2, -1, 0, +1, +2 )
    float Exposure;

    // The maximum luminance to use for linear tonemapping.
    float MaxLuminance;

    // Reinhard constant. Generally this is 1.0.
    float K;

    // ACES Filmic parameters
    // See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
    float A; // Shoulder strength
    float B; // Linear strength
    float C; // Linear angle
    float D; // Toe strength
    float E; // Toe Numerator
    float F; // Toe denominator
    // Note E/F = Toe angle.
    float LinearWhite;
    float Gamma;
  };

  TonemapParameters g_TonemapParameters;

  // An enum for root signature parameters.
  // I'm not using scoped enums to avoid the explicit cast that would be required
  // to use these as root indices in the root signature.
  enum RootParameters {
    MatricesCBTutorial,         // ConstantBuffer<Mat> MatCB : register(b0);
    MaterialCBTutorial,         // ConstantBuffer<Material> MaterialCB : register( b0, space1 );
    LightPropertiesCBTutorial,  // ConstantBuffer<LightProperties> LightPropertiesCB : register( b1 );
    PointLightsTutorial,        // StructuredBuffer<PointLight> PointLights : register( t0 );
    SpotLightsTutorial,         // StructuredBuffer<SpotLight> SpotLights : register( t1 );
    TexturesTutorial,           // Texture2D DiffuseTexture : register( t2 );
    NumRootParametersTutorial
  };

  // Builds a look-at (world) matrix from a point, up and direction vectors.
  XMMATRIX XM_CALLCONV LookAtMatrix(FXMVECTOR Position, FXMVECTOR Direction, FXMVECTOR Up) {
    assert(!XMVector3Equal(Direction, XMVectorZero()));
    assert(!XMVector3IsInfinite(Direction));
    assert(!XMVector3Equal(Up, XMVectorZero()));
    assert(!XMVector3IsInfinite(Up));

    XMVECTOR R2 = XMVector3Normalize(Direction);

    XMVECTOR R0 = XMVector3Cross(Up, R2);
    R0 = XMVector3Normalize(R0);

    XMVECTOR R1 = XMVector3Cross(R2, R0);

    XMMATRIX M(R0, R1, R2, Position);

    return M;
  }

  Tutorial4::Tutorial4(const std::wstring& name, int width, int height, bool vSync)
    : super(name, width, height, vSync)
    , m_ScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX))
    , m_Forward(0)
    , m_Backward(0)
    , m_Left(0)
    , m_Right(0)
    , m_Up(0)
    , m_Down(0)
    , m_Pitch(0)
    , m_Yaw(0)
    , m_AnimateLights(false)
    , m_Shift(false)
    , m_Width(0)
    , m_Height(0)
    , m_RenderScale(1.0f) {

    Kame::Math::Vector4 cameraPos1(0, 5, -20, 1);
    XMVECTOR cameraPos = XMVectorSet(0, 5, -20, 1);
    XMVECTOR cameraTarget = XMVectorSet(0, 5, 0, 1);
    XMVECTOR cameraUp = XMVectorSet(0, 1, 0, 0);

    m_Camera.set_LookAt(cameraPos, cameraTarget, cameraUp);
    m_Camera.set_Projection(45.0f, width / (float)height, 0.1f, 100.0f);

    m_pAlignedCameraData = (CameraData*)_aligned_malloc(sizeof(CameraData), 16);

    m_pAlignedCameraData->m_InitialCamPos = m_Camera.get_Translation();
    m_pAlignedCameraData->m_InitialCamRot = m_Camera.get_Rotation();
    m_pAlignedCameraData->m_InitialFov = m_Camera.get_FoV();

    Input::KeyEvent(Kame::Key::F)->AddHandler("Tutorial4::OnKeyF", BIND_FUNCTION(Tutorial4::OnKeyF));
    Input::MapKeyToCustomEvent(Kame::Key::G, L"Shoot");
    Input::CustomEvent(L"Shoot")->AddHandler("Tutorial4::Shoot", BIND_FUNCTION(Tutorial4::Shoot));

    m_HDRRenderTarget.reset(GraphicsCore::CreateRenderTarget());

    _SkyboxProgram.reset(new RenderProgramDx12());
    _HDRProgram.reset(new RenderProgramDx12());
    _SDRProgram.reset(new RenderProgramDx12());

    m_SkyboxSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    m_HDRRootSignature = GraphicsCore::CreateRenderProgramSignatureNc();
    m_SDRRootSignature = GraphicsCore::CreateRenderProgramSignatureNc();

  }

  Tutorial4::~Tutorial4() {
    _aligned_free(m_pAlignedCameraData);
  }

  bool Tutorial4::LoadContent() {
    auto device = DX12Core::Get().GetDevice();
    auto commandQueue = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto commandList = commandQueue->GetCommandList();

    // Create a Cube mesh
    m_CubeMesh = MeshFactory::GetCube();
    m_SphereMesh = MeshFactory::GetSphere();
    m_ConeMesh = MeshFactory::GetCone();
    m_TorusMesh = MeshFactory::GetTorus();
    //m_TorusMesh = MeshFactory::GetPlane();
    m_PlaneMesh = MeshFactory::GetPlane();

    m_DebugCube = MeshFactory::GetDebugCube();
    // Create an inverted (reverse winding order) cube so the insides are not clipped.
    m_SkyboxMesh = MeshFactory::GetCube(1.0f, true);
    //m_SkyboxMesh = Mesh::CreateCube(*commandList, 1.0f, true);

    // Load some textures

    //commandList->LoadTextureFromFile(m_DefaultTexture, L"Assets/Textures/DefaultWhite.bmp");
    m_DefaultTexture = TextureManager::GetTexture(L"Assets/Textures/DefaultWhite.bmp");
    m_DirectXTexture = TextureManager::GetTexture(L"Assets/Textures/Directx9.png");
    m_EarthTexture = TextureManager::GetTexture(L"Assets/Textures/earth.dds");
    m_MonaLisaTexture = TextureManager::GetTexture(L"Assets/Textures/Mona_Lisa.jpg");
    m_GraceCathedralTexture = TextureManager::GetTexture(L"Assets/Textures/grace-new.hdr");
    //    commandList->LoadTextureFromFile(m_GraceCathedralTexture, L"Assets/Textures/UV_Test_Pattern.png");

        // Create a cubemap for the HDR panorama.
    m_GraceCathedralCubemap = m_GraceCathedralTexture->ToCubeMap(1024,L"Grace Cathedral Cubemap");

    // Create an HDR intermediate render target.
    DXGI_FORMAT HDRFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

    // Create an off-screen render target with a single color buffer and a depth buffer.
    auto colorDesc = CD3DX12_RESOURCE_DESC::Tex2D(HDRFormat, m_Width, m_Height);
    colorDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE colorClearValue;
    colorClearValue.Format = colorDesc.Format;
    colorClearValue.Color[0] = 0.4f;
    colorClearValue.Color[1] = 0.6f;
    colorClearValue.Color[2] = 0.9f;
    colorClearValue.Color[3] = 1.0f;

    _HDRTexture = GraphicsCore::CreateTexture(
      colorDesc, &colorClearValue,
      TextureUsage::RenderTarget,
      L"HDR Texture"
    );

    // Create a depth buffer for the HDR render target.
    auto depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthBufferFormat, m_Width, m_Height);
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE depthClearValue;
    depthClearValue.Format = depthDesc.Format;
    depthClearValue.DepthStencil = { 1.0f, 0 };

    _DepthTexture = GraphicsCore::CreateTexture(depthDesc, &depthClearValue, TextureUsage::Depth, L"Depth Render Target");

    // Attach the HDR texture to the HDR render target.
    m_HDRRenderTarget->AttachTexture(AttachmentPoint::Color0, _HDRTexture.get());
    m_HDRRenderTarget->AttachTexture(AttachmentPoint::DepthStencil, _DepthTexture.get());

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
      featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    {
      // Load the Skybox shaders.
      ComPtr<ID3DBlob> vs;
      ComPtr<ID3DBlob> ps;
      ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Skybox_VS.cso", &vs));
      ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\Skybox_PS.cso", &ps));

      // Setup the input layout for the skybox vertex shader.
      D3D12_INPUT_ELEMENT_DESC inputLayout[1] = {
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
      };

      // Allow input layout and deny unnecessary access to certain pipeline stages.
      D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

      CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

      CD3DX12_ROOT_PARAMETER1 rootParameters[2];
      rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
      rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

      CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
      rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampSampler, rootSignatureFlags);

      //m_SkyboxSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
      m_SkyboxSignature->SetDescription(rootSignatureDescription.Desc_1_1);

      _SkyboxProgram->SetRootSignature(m_SkyboxSignature.get());
      _SkyboxProgram->SetInputLayout(1, inputLayout);
      _SkyboxProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      _SkyboxProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
      _SkyboxProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
      _SkyboxProgram->SetRenderTargetFormats(m_HDRRenderTarget->GetRenderTargetFormats());

      _SkyboxProgram->Create();
    }






    // Create a root signature for the HDR pipeline.
    {
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

      CD3DX12_ROOT_PARAMETER1 rootParameters[RootParameters::NumRootParametersTutorial];
      rootParameters[RootParameters::MatricesCBTutorial].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
      rootParameters[RootParameters::MaterialCBTutorial].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
      rootParameters[RootParameters::LightPropertiesCBTutorial].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
      rootParameters[RootParameters::PointLightsTutorial].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
      rootParameters[RootParameters::SpotLightsTutorial].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
      rootParameters[RootParameters::TexturesTutorial].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

      CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
      CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
      rootSignatureDescription.Init_1_1(RootParameters::NumRootParametersTutorial, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

      //m_HDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
      m_HDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1);

      _HDRProgram->SetRootSignature(m_HDRRootSignature.get());
      _HDRProgram->SetInputLayout(VertexPositionNormalTexture::InputElementCount, VertexPositionNormalTexture::InputElements);
      _HDRProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      _HDRProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
      _HDRProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
      _HDRProgram->SetRenderTargetFormats(m_HDRRenderTarget->GetRenderTargetFormats(), m_HDRRenderTarget->GetDepthStencilFormat());

      _HDRProgram->Create();
    }

    // Create the SDR Root Signature
    {
      CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

      CD3DX12_ROOT_PARAMETER1 rootParameters[2];
      rootParameters[0].InitAsConstants(sizeof(TonemapParameters) / 4, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
      rootParameters[1].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

      CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

      CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
      rootSignatureDescription.Init_1_1(2, rootParameters, 1, &linearClampsSampler);

      //m_SDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
      m_SDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1);

      //Test RootSignature Hash
      {

        CD3DX12_DESCRIPTOR_RANGE1 descriptorRange2(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER1 rootParameters2[2];
        rootParameters2[0].InitAsConstants(sizeof(TonemapParameters) / 4, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters2[1].InitAsDescriptorTable(1, &descriptorRange2, D3D12_SHADER_VISIBILITY_PIXEL);

        CD3DX12_STATIC_SAMPLER_DESC linearClampsSampler2(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription2;
        rootSignatureDescription2.Init_1_1(2, rootParameters2, 1, &linearClampsSampler2);

        RootSignatureDx12 rootSignature2;
        //rootSignature2.SetDescription(rootSignatureDescription2.Desc_1_1, featureData.HighestVersion);
        rootSignature2.SetDescription(rootSignatureDescription2.Desc_1_1);

      } //End Test RootSignature Hash

      // Create the SDR PSO
      ComPtr<ID3DBlob> vs;
      ComPtr<ID3DBlob> ps;
      ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDRtoSDR_VS.cso", &vs));
      ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDRtoSDR_PS.cso", &ps));

      CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);
      rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

      _SDRProgram->SetRootSignature(m_SDRRootSignature.get());
      _SDRProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      _SDRProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
      _SDRProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
      _SDRProgram->SetRasterizer(rasterizerDesc);
      _SDRProgram->SetRenderTargetFormats(_Window->GetDisplay().GetRenderTarget().GetRenderTargetFormats());

      _SDRProgram->Create();


      //Test Hash
      Reference<RenderProgramDx12> testProgram;
      testProgram.reset(new RenderProgramDx12());
      testProgram->SetRootSignature(m_SDRRootSignature.get());
      testProgram->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
      testProgram->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
      testProgram->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
      testProgram->SetRasterizer(rasterizerDesc);
      testProgram->SetRenderTargetFormats(_Window->GetDisplay().GetRenderTarget().GetRenderTargetFormats());
      testProgram->Create();

    }

    auto fenceValue = commandQueue->ExecuteCommandList(commandList);
    commandQueue->WaitForFenceValue(fenceValue);

    return true;
  }

  void Tutorial4::RescaleHDRRenderTarget(float scale) {
    uint32_t width = static_cast<uint32_t>(m_Width * scale);
    uint32_t height = static_cast<uint32_t>(m_Height * scale);

    width = Kame::Math::clamp<uint32_t>(width, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);
    height = Kame::Math::clamp<uint32_t>(height, 1, D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION);

    m_HDRRenderTarget->Resize(width, height);
  }

  void Tutorial4::OnResize(ResizeEventArgs& e) {
    super::OnResize(e);

    if (m_Width != e.Width || m_Height != e.Height) {
      m_Width = std::max(1, e.Width);
      m_Height = std::max(1, e.Height);

      float fov = m_Camera.get_FoV();
      float aspectRatio = m_Width / (float)m_Height;
      m_Camera.set_Projection(fov, aspectRatio, 0.1f, 100.0f);

      RescaleHDRRenderTarget(m_RenderScale);
    }
  }

  void Tutorial4::UnloadContent() {}

  static double g_FPS = 0.0;

  void Tutorial4::OnUpdate(UpdateEventArgs& e) {
    static uint64_t frameCount = 0;
    static double totalTime = 0.0;

    super::OnUpdate(e);

    totalTime += e.ElapsedTime;
    frameCount++;

    if (totalTime > 1.0) {
      g_FPS = frameCount / totalTime;

      char buffer[512];
      sprintf_s(buffer, "FPS: %f\n", g_FPS);
      OutputDebugStringA(buffer);

      frameCount = 0;
      totalTime = 0.0;
    }

    // Update the camera.
    float speedMultipler = (m_Shift ? 16.0f : 4.0f);

    XMVECTOR cameraTranslate = XMVectorSet(m_Right - m_Left, 0.0f, m_Forward - m_Backward, 1.0f) * speedMultipler * static_cast<float>(e.ElapsedTime);
    XMVECTOR cameraPan = XMVectorSet(0.0f, m_Up - m_Down, 0.0f, 1.0f) * speedMultipler * static_cast<float>(e.ElapsedTime);
    m_Camera.Translate(cameraTranslate, Space::Local);
    m_Camera.Translate(cameraPan, Space::Local);

    XMVECTOR cameraRotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), 0.0f);
    m_Camera.set_Rotation(cameraRotation);

    Kame::Math::Matrix4x4 viewMatrix = m_Camera.get_ViewMatrix();

    const int numPointLights = 4;
    const int numSpotLights = 4;

    static const XMVECTORF32 LightColors[] =
    {
        Colors::White, Colors::Orange, Colors::Yellow, Colors::Green, Colors::Blue, Colors::Indigo, Colors::Violet, Colors::White
    };

    static float lightAnimTime = 0.0f;
    if (m_AnimateLights) {
      lightAnimTime += static_cast<float>(e.ElapsedTime) * 0.5f * XM_PI;
    }

    const float radius = 8.0f;
    const float offset = 2.0f * XM_PI / numPointLights;
    const float offset2 = offset + (offset / 2.0f);

    // Setup the light buffers.
    m_PointLights.resize(numPointLights);
    for (int i = 0; i < numPointLights; ++i) {
      PointLight& l = m_PointLights[i];

      l.PositionWS = {
          static_cast<float>(std::sin(lightAnimTime + offset * i))* radius,
          9.0f,
          static_cast<float>(std::cos(lightAnimTime + offset * i))* radius,
          1.0f
      };
      Kame::Math::Vector4 positionWS = XMLoadFloat4(&l.PositionWS);
      Kame::Math::Vector4 positionVS = Kame::Math::Vector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS.GetXmVector());

      l.Color = XMFLOAT4(LightColors[i]);
      l.Intensity = 1.0f;
      l.Attenuation = 0.0f;
    }

    m_SpotLights.resize(numSpotLights);
    for (int i = 0; i < numSpotLights; ++i) {
      SpotLight& l = m_SpotLights[i];

      l.PositionWS = {
          static_cast<float>(std::sin(lightAnimTime + offset * i + offset2))* radius,
          9.0f,
          static_cast<float>(std::cos(lightAnimTime + offset * i + offset2))* radius,
          1.0f
      };
      Kame::Math::Vector4 positionWS = XMLoadFloat4(&l.PositionWS);
      Kame::Math::Vector4 positionVS = Kame::Math::Vector3TransformCoord(positionWS, viewMatrix);
      XMStoreFloat4(&l.PositionVS, positionVS.GetXmVector());

      Kame::Math::Vector4 directionWS = XMVector3Normalize(XMVectorSetW(XMVectorNegate(positionWS.GetXmVector()), 0));
      Kame::Math::Vector4 directionVS = XMVector3Normalize(XMVector3TransformNormal(directionWS.GetXmVector(), viewMatrix.GetXmMatrix()));
      XMStoreFloat4(&l.DirectionWS, directionWS.GetXmVector());
      XMStoreFloat4(&l.DirectionVS, directionVS.GetXmVector());

      l.Color = XMFLOAT4(LightColors[numPointLights + i]);
      l.Intensity = 1.0f;
      l.SpotAngle = XMConvertToRadians(45.0f);
      l.Attenuation = 0.0f;
    }
  }

  // Helper to display a little (?) mark which shows a tooltip when hovered.
  static void ShowHelpMarker(const char* desc) {
    //ImGui::TextDisabled("(?)");
    //if (ImGui::IsItemHovered())
    //{
    //    ImGui::BeginTooltip();
    //    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    //    ImGui::TextUnformatted(desc);
    //    ImGui::PopTextWrapPos();
    //    ImGui::EndTooltip();
    //}
  }

  // Number of values to plot in the tonemapping curves.
  static const int VALUES_COUNT = 256;
  // Maximum HDR value to normalize the plot samples.
  static const float HDR_MAX = 12.0f;

  float LinearTonemapping(float HDR, float max) {
    if (max > 0.0f) {
      return Kame::Math::clamp(HDR / max);
    }
    return HDR;
  }

  float LinearTonemappingPlot(void*, int index) {
    return LinearTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.MaxLuminance);
  }

  // Reinhard tone mapping.
  // See: http://www.cs.utah.edu/~reinhard/cdrom/tonemap.pdf
  float ReinhardTonemapping(float HDR, float k) {
    return HDR / (HDR + k);
  }

  float ReinhardTonemappingPlot(void*, int index) {
    return ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
  }

  float ReinhardSqrTonemappingPlot(void*, int index) {
    float reinhard = ReinhardTonemapping(index / (float)VALUES_COUNT * HDR_MAX, g_TonemapParameters.K);
    return reinhard * reinhard;
  }

  // ACES Filmic
  // See: https://www.slideshare.net/ozlael/hable-john-uncharted2-hdr-lighting/142
  float ACESFilmicTonemapping(float x, float A, float B, float C, float D, float E, float F) {
    return (((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - (E / F));
  }

  float ACESFilmicTonemappingPlot(void*, int index) {
    float HDR = index / (float)VALUES_COUNT * HDR_MAX;
    return ACESFilmicTonemapping(HDR, g_TonemapParameters.A, g_TonemapParameters.B, g_TonemapParameters.C, g_TonemapParameters.D, g_TonemapParameters.E, g_TonemapParameters.F) /
      ACESFilmicTonemapping(g_TonemapParameters.LinearWhite, g_TonemapParameters.A, g_TonemapParameters.B, g_TonemapParameters.C, g_TonemapParameters.D, g_TonemapParameters.E, g_TonemapParameters.F);
  }

  void Tutorial4::OnGUI() {
    //static bool showDemoWindow = false;
    //static bool showOptions = true;

    //if (ImGui::BeginMainMenuBar())
    //{
    //    if (ImGui::BeginMenu("File"))
    //    {
    //        if (ImGui::MenuItem("Exit", "Esc"))
    //        {
    //            Application::Get().Quit();
    //        }
    //        ImGui::EndMenu();
    //    }

    //    if (ImGui::BeginMenu("View"))
    //    {
    //        ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
    //        ImGui::MenuItem("Tonemapping", nullptr, &showOptions);

    //        ImGui::EndMenu();
    //    }

    //    if (ImGui::BeginMenu("Options") )
    //    {
    //        bool vSync = m_pWindow->IsVSync();
    //        if (ImGui::MenuItem("V-Sync", "V", &vSync))
    //        {
    //            m_pWindow->SetVSync(vSync);
    //        }

    //        bool fullscreen = m_pWindow->IsFullScreen();
    //        if (ImGui::MenuItem("Full screen", "Alt+Enter", &fullscreen) )
    //        {
    //            m_pWindow->SetFullscreen(fullscreen);
    //        }

    //        ImGui::EndMenu();
    //    }

    //    char buffer[256];

    //    {
    //        // Output a slider to scale the resolution of the HDR render target.
    //        float renderScale = m_RenderScale;
    //        ImGui::PushItemWidth(300.0f);
    //        ImGui::SliderFloat("Resolution Scale", &renderScale, 0.1f, 2.0f);
    //        // Using Ctrl+Click on the slider, the user can set values outside of the 
    //        // specified range. Make sure to clamp to a sane range to avoid creating
    //        // giant render targets.
    //        renderScale = clamp(renderScale, 0.0f, 2.0f);

    //        // Output current resolution of render target.
    //        auto size = m_HDRRenderTarget.GetSize();
    //        ImGui::SameLine();
    //        sprintf_s(buffer, _countof(buffer), "(%ux%u)", size.x, size.y);
    //        ImGui::Text(buffer);

    //        // Resize HDR render target if the scale changed.
    //        if (renderScale != m_RenderScale)
    //        {
    //            m_RenderScale = renderScale;
    //            RescaleHDRRenderTarget(m_RenderScale);
    //        }
    //    }

    //    {
    //        sprintf_s(buffer, _countof(buffer), "FPS: %.2f (%.2f ms)  ", g_FPS, 1.0 / g_FPS * 1000.0);
    //        auto fpsTextSize = ImGui::CalcTextSize(buffer);
    //        ImGui::SameLine(ImGui::GetWindowWidth() - fpsTextSize.x);
    //        ImGui::Text(buffer);
    //    }

    //    ImGui::EndMainMenuBar();
    //}

    //if (showDemoWindow)
    //{
    //    ImGui::ShowDemoWindow(&showDemoWindow);
    //}

    //if (showOptions)
    //{
    //    ImGui::Begin("Tonemapping", &showOptions);
    //    {
    //        ImGui::TextWrapped("Use the Exposure slider to adjust the overall exposure of the HDR scene.");
    //        ImGui::SliderFloat("Exposure", &g_TonemapParameters.Exposure, -10.0f, 10.0f);
    //        ImGui::SameLine(); ShowHelpMarker("Adjust the overall exposure of the HDR scene.");
    //        ImGui::SliderFloat("Gamma", &g_TonemapParameters.Gamma, 0.01f, 5.0f);
    //        ImGui::SameLine(); ShowHelpMarker("Adjust the Gamma of the output image.");

    //        const char* toneMappingMethods[] = {
    //            "Linear",
    //            "Reinhard",
    //            "Reinhard Squared",
    //            "ACES Filmic"
    //        };

    //        ImGui::Combo("Tonemapping Methods", (int*)(&g_TonemapParameters.TonemapMethod), toneMappingMethods, 4);

    //        switch (g_TonemapParameters.TonemapMethod)
    //        {
    //        case TonemapMethod::TM_Linear:
    //            ImGui::PlotLines("Linear Tonemapping", &LinearTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Max Brightness", &g_TonemapParameters.MaxLuminance, 1.0f, 10.0f);
    //            ImGui::SameLine(); ShowHelpMarker("Linearly scale the HDR image by the maximum brightness.");
    //            break;
    //        case TonemapMethod::TM_Reinhard:
    //            ImGui::PlotLines("Reinhard Tonemapping", &ReinhardTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
    //            ImGui::SameLine(); ShowHelpMarker("The Reinhard constant is used in the denominator.");
    //            break;
    //        case TonemapMethod::TM_ReinhardSq:
    //            ImGui::PlotLines("Reinhard Squared Tonemapping", &ReinhardSqrTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Reinhard Constant", &g_TonemapParameters.K, 0.01f, 10.0f);
    //            ImGui::SameLine(); ShowHelpMarker("The Reinhard constant is used in the denominator.");
    //            break;
    //        case TonemapMethod::TM_ACESFilmic:
    //            ImGui::PlotLines("ACES Filmic Tonemapping", &ACESFilmicTonemappingPlot, nullptr, VALUES_COUNT, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 250));
    //            ImGui::SliderFloat("Shoulder Strength", &g_TonemapParameters.A, 0.01f, 5.0f);
    //            ImGui::SliderFloat("Linear Strength", &g_TonemapParameters.B, 0.0f, 100.0f);
    //            ImGui::SliderFloat("Linear Angle", &g_TonemapParameters.C, 0.0f, 1.0f);
    //            ImGui::SliderFloat("Toe Strength", &g_TonemapParameters.D, 0.01f, 1.0f);
    //            ImGui::SliderFloat("Toe Numerator", &g_TonemapParameters.E, 0.0f, 10.0f);
    //            ImGui::SliderFloat("Toe Denominator", &g_TonemapParameters.F, 1.0f, 10.0f);
    //            ImGui::SliderFloat("Linear White", &g_TonemapParameters.LinearWhite, 1.0f, 120.0f);
    //            break;
    //        default:
    //            break;
    //        }
    //    }

    //    if (ImGui::Button("Reset to Defaults"))
    //    {
    //        TonemapMethod method = g_TonemapParameters.TonemapMethod;
    //        g_TonemapParameters = TonemapParameters();
    //        g_TonemapParameters.TonemapMethod = method;
    //    }

    //    ImGui::End();

    //}
  }

  void XM_CALLCONV ComputeMatrices(FXMMATRIX model, CXMMATRIX view, CXMMATRIX viewProjection, Mat& mat) {
    mat.ModelMatrix = model;
    mat.ModelViewMatrix = model * view;
    mat.InverseTransposeModelViewMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, mat.ModelViewMatrix));
    mat.ModelViewProjectionMatrix = model * viewProjection;
  }

  void Tutorial4::OnRender(RenderEventArgs& e) {
    super::OnRender(e);

    //auto commandQueue = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //auto commandList = commandQueue->GetCommandList();

    //std::shared_ptr<CommandListDx12> commandList = GraphicsCore::BeginCommandListDx(D3D12_COMMAND_LIST_TYPE_DIRECT);
    Reference<CommandList> commandListBase = GraphicsCore::BeginCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
    //CommandList* commandListBase = static_cast<CommandList*>(commandList.get());

    // Clear the render targets.
    {
      FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

      commandListBase->ClearTexture(m_HDRRenderTarget->GetTexture(AttachmentPoint::Color0), clearColor);
      commandListBase->ClearDepthStencilTexture(m_HDRRenderTarget->GetTexture(AttachmentPoint::DepthStencil), D3D12_CLEAR_FLAG_DEPTH);
    }

    commandListBase->SetRenderTarget(*m_HDRRenderTarget);
    D3D12_VIEWPORT& vp = m_HDRRenderTarget->GetViewport();
    //vp.Width /= 2;
    commandListBase->SetViewport(vp);
    commandListBase->SetScissorRect(m_ScissorRect);

    // Render the skybox.
    {
      // The view matrix should only consider the camera's rotation, but not the translation.
      auto viewMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(m_Camera.get_Rotation().GetXmVector()));
      auto projMatrix = m_Camera.get_ProjectionMatrix().GetXmMatrix();
      DirectX::XMMATRIX viewProjMatrix = viewMatrix * projMatrix;

      //commandList->SetPipelineState(m_SkyboxPipelineState);
      commandListBase->SetRenderProgram(_SkyboxProgram.get());
      commandListBase->SetGraphicsRootSignature(m_SkyboxSignature.get());

      commandListBase->SetGraphics32BitConstants(0, viewProjMatrix);

      D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
      //srvDesc.Format = m_GraceCathedralCubemap.GetD3D12ResourceDesc().Format;
      srvDesc.Format = m_GraceCathedralCubemap->GetFormat();
      srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
      srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
      srvDesc.TextureCube.MipLevels = (UINT)-1; // Use all mips.

      // TODO: Need a better way to bind a cubemap.
      commandListBase->SetShaderResourceViewTexture(1, 0, m_GraceCathedralCubemap.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, &srvDesc);

      m_SkyboxMesh->Draw(commandListBase.get());
    }


    commandListBase->SetRenderProgram(_HDRProgram.get());
    commandListBase->SetGraphicsRootSignature(m_HDRRootSignature.get());

    // Upload lights
    LightProperties lightProps;
    lightProps.NumPointLights = static_cast<uint32_t>(m_PointLights.size());
    lightProps.NumSpotLights = static_cast<uint32_t>(m_SpotLights.size());

    commandListBase->SetGraphics32BitConstants(RootParameters::LightPropertiesCBTutorial, lightProps);
    commandListBase->SetGraphicsDynamicStructuredBuffer(RootParameters::PointLightsTutorial, m_PointLights);
    commandListBase->SetGraphicsDynamicStructuredBuffer(RootParameters::SpotLightsTutorial, m_SpotLights);

    // Draw the earth sphere
    XMMATRIX translationMatrix = XMMatrixTranslation(-4.0f, 2.0f, -4.0f);
    XMMATRIX rotationMatrix = XMMatrixIdentity();
    XMMATRIX scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
    XMMATRIX worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
    XMMATRIX viewMatrix = m_Camera.get_ViewMatrix().GetXmMatrix();
    XMMATRIX viewProjectionMatrix = viewMatrix * m_Camera.get_ProjectionMatrix().GetXmMatrix();

    Mat matrices;
    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::White);
    commandListBase->SetShaderResourceViewTexture(RootParameters::TexturesTutorial, 0, m_EarthTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_SphereMesh->Draw(commandListBase.get());

    // Draw a cube
    translationMatrix = XMMatrixTranslation(4.0f, 4.0f, 4.0f);
    rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
    scaleMatrix = XMMatrixScaling(4.0f, 8.0f, 4.0f);
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::White);
    commandListBase->SetShaderResourceViewTexture(RootParameters::TexturesTutorial, 0, m_MonaLisaTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_CubeMesh->Draw(commandListBase.get());

    //vp.Width *= 2;
    //vp.TopLeftX += vp.Width;
    commandListBase->SetViewport(vp);
    // Draw a torus
    translationMatrix = XMMatrixTranslation(4.0f, 0.6f, -4.0f);
    rotationMatrix = XMMatrixRotationY(XMConvertToRadians(45.0f));
    scaleMatrix = XMMatrixScaling(4.0f, 4.0f, 4.0f);
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::Ruby);
    commandListBase->SetShaderResourceViewTexture(RootParameters::TexturesTutorial, 0, m_DefaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_TorusMesh->Draw(commandListBase.get());

    // Floor plane.
    float scalePlane = 20.0f;
    float translateOffset = scalePlane / 2.0f;

    translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    rotationMatrix = XMMatrixIdentity();
    scaleMatrix = XMMatrixScaling(scalePlane, 1.0f, scalePlane);
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::White);
    commandListBase->SetShaderResourceViewTexture(RootParameters::TexturesTutorial, 0, m_DirectXTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_PlaneMesh->Draw(commandListBase.get());

    // Back wall
    translationMatrix = XMMatrixTranslation(0, translateOffset, translateOffset);
    rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90));
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);

    m_PlaneMesh->Draw(commandListBase.get());

    // Ceiling plane
    translationMatrix = XMMatrixTranslation(0, translateOffset * 2.0f, 0);
    rotationMatrix = XMMatrixRotationX(XMConvertToRadians(180));
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);

    m_PlaneMesh->Draw(commandListBase.get());

    // Front wall
    translationMatrix = XMMatrixTranslation(0, translateOffset, -translateOffset);
    rotationMatrix = XMMatrixRotationX(XMConvertToRadians(90));
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);

    m_PlaneMesh->Draw(commandListBase.get());

    // Left wall
    translationMatrix = XMMatrixTranslation(-translateOffset, translateOffset, 0);
    rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(-90));
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::Red);
    commandListBase->SetShaderResourceViewTexture(RootParameters::TexturesTutorial, 0, m_DefaultTexture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_PlaneMesh->Draw(commandListBase.get());

    // Right wall
    translationMatrix = XMMatrixTranslation(translateOffset, translateOffset, 0);
    rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * XMMatrixRotationY(XMConvertToRadians(90));
    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
    commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, BaseMaterialParameters::Blue);
    m_PlaneMesh->Draw(commandListBase.get());

    // Draw shapes to visualize the position of the lights in the scene.
    BaseMaterialParameters lightMaterial;
    // No specular
    lightMaterial.Specular = { 0, 0, 0, 1 };
    for (const auto& l : m_PointLights) {
      lightMaterial.Emissive = l.Color;
      XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
      worldMatrix = XMMatrixTranslationFromVector(lightPos);
      ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

      commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
      commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, lightMaterial);

      m_SphereMesh->Draw(commandListBase.get());




      m_DebugCube->Draw(commandListBase.get());
    }

    for (const auto& l : m_SpotLights) {
      lightMaterial.Emissive = l.Color;
      XMVECTOR lightPos = XMLoadFloat4(&l.PositionWS);
      XMVECTOR lightDir = XMLoadFloat4(&l.DirectionWS);
      XMVECTOR up = XMVectorSet(0, 1, 0, 0);

      // Rotate the cone so it is facing the Z axis.
      rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-90.0f));
      worldMatrix = rotationMatrix * LookAtMatrix(lightPos, lightDir, up);

      ComputeMatrices(worldMatrix, viewMatrix, viewProjectionMatrix, matrices);

      commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MatricesCBTutorial, matrices);
      commandListBase->SetGraphicsDynamicConstantBuffer(RootParameters::MaterialCBTutorial, lightMaterial);

      m_ConeMesh->Draw(commandListBase.get());
    }

    // Perform HDR -> SDR tonemapping directly to the Window's render target.
    commandListBase->SetRenderTarget(_Window->GetDisplay().GetRenderTarget());
    commandListBase->SetViewport(_Window->GetDisplay().GetRenderTarget().GetViewport());
    commandListBase->SetRenderProgram(_SDRProgram.get());
    commandListBase->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandListBase->SetGraphicsRootSignature(m_SDRRootSignature.get());
    commandListBase->SetGraphics32BitConstants(0, g_TonemapParameters);
    commandListBase->SetShaderResourceViewTexture(1, 0, m_HDRRenderTarget->GetTexture(Color0), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandListBase->Draw(3);

    //commandQueue->ExecuteCommandList(commandList);
    GraphicsCore::ExecuteCommandList(commandListBase);
    //commandList->Execute();

    // Render GUI.
    OnGUI();

    // Present
    _Window->GetDisplay().Present();
  }

  static bool g_AllowFullscreenToggle = true;

  void Tutorial4::OnKeyPressed(KeyEventArgs& e) {
    super::OnKeyPressed(e);

    /*if (!ImGui::GetIO().WantCaptureKeyboard)
    {*/
    switch (e.Key) {
    case Kame::Key::Escape:
      Application::Get().Quit(0);
      break;
    case Kame::Key::ENTER:
      if (e.Alt) {
    case Kame::Key::F11:
      if (g_AllowFullscreenToggle) {
        _Window->ToggleFullscreen();
        g_AllowFullscreenToggle = false;
      }
      break;
      }
    case Kame::Key::V:
      _Window->GetDisplay().ToggleVSync();
      break;
    case Kame::Key::R:
      // Reset camera transform
      m_Camera.set_Translation(m_pAlignedCameraData->m_InitialCamPos);
      m_Camera.set_Rotation(m_pAlignedCameraData->m_InitialCamRot);
      m_Camera.set_FoV(m_pAlignedCameraData->m_InitialFov);
      m_Pitch = 0.0f;
      m_Yaw = 0.0f;
      break;
    case Kame::Key::UP:
    case Kame::Key::W:
      m_Forward = 1.0f;
      break;
    case Kame::Key::LEFT:
    case Kame::Key::A:
      m_Left = 1.0f;
      break;
    case Kame::Key::DOWN:
    case Kame::Key::S:
      m_Backward = 1.0f;
      break;
    case Kame::Key::RIGHT:
    case Kame::Key::D:
      m_Right = 1.0f;
      break;
    case Kame::Key::Q:
      m_Down = 1.0f;
      break;
    case Kame::Key::E:
      m_Up = 1.0f;
      break;
    case Kame::Key::SPACE:
      m_AnimateLights = !m_AnimateLights;
      break;
    case Kame::Key::LSHIFT:
      m_Shift = true;
      break;
    }
    //}
  }

  bool Tutorial4::OnKeyF(KeyEventArgs& e) {
    if (e.State == KeyEventArgs::KeyState::Released) {
      _Window->ToggleFullscreen();
    }
    return true;
  }

  bool Tutorial4::Shoot(KeyEventArgs& e) {
    if (e.State == KeyEventArgs::KeyState::Released) {
      _Window->ToggleFullscreen();
    }
    return true;
  }

  void Tutorial4::OnKeyReleased(KeyEventArgs& e) {
    super::OnKeyReleased(e);

    /*if (!ImGui::GetIO().WantCaptureKeyboard)
    {*/
    switch (e.Key) {
    case Kame::Key::ENTER:
      if (e.Alt) {
    case Kame::Key::F11:
      g_AllowFullscreenToggle = true;
      }
      break;
    case Kame::Key::UP:
    case Kame::Key::W:
      m_Forward = 0.0f;
      break;
    case Kame::Key::LEFT:
    case Kame::Key::A:
      m_Left = 0.0f;
      break;
    case Kame::Key::DOWN:
    case Kame::Key::S:
      m_Backward = 0.0f;
      break;
    case Kame::Key::RIGHT:
    case Kame::Key::D:
      m_Right = 0.0f;
      break;
    case Kame::Key::Q:
      m_Down = 0.0f;
      break;
    case Kame::Key::E:
      m_Up = 0.0f;
      break;
    case Kame::Key::LSHIFT:
      m_Shift = false;
      break;
    }
    //}
  }

  void Tutorial4::OnMouseMoved(MouseMotionEventArgs& e) {
    super::OnMouseMoved(e);

    const float mouseSpeed = 0.1f;
    //if (!ImGui::GetIO().WantCaptureMouse)
    //{
    if (e.LeftButton) {
      m_Pitch -= e.RelY * mouseSpeed;

      m_Pitch = Kame::Math::clamp(m_Pitch, -90.0f, 90.0f);

      m_Yaw -= e.RelX * mouseSpeed;
    }
    //}
  }


  void Tutorial4::OnMouseWheel(MouseWheelEventArgs& e) {
    //if (!ImGui::GetIO().WantCaptureMouse)
    //{
    auto fov = m_Camera.get_FoV();

    fov -= e.WheelDelta;
    fov = Kame::Math::clamp(fov, 12.0f, 90.0f);

    m_Camera.set_FoV(fov);

    char buffer[256];
    sprintf_s(buffer, "FoV: %f\n", fov);
    OutputDebugStringA(buffer);
    //}
  }

}