#include "kmpch.h"
#include "Skybox.h"

#include <Kame/Core/DebugUtilities.h>
#include "Material.h"
#include "Mesh.h"
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/RenderApi/RenderProgram.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/RenderApi/Texture.h>
#include <Kame\Graphics\MaterialManager.h>
#include <Kame\Graphics\MeshFactory.h>

namespace Kame {

  Skybox::Skybox() {
    _Material = MaterialManager::GetMaterial<SkyboxMaterial>();
    _MaterialInstance = MaterialInstance<SkyboxMaterialParameters>::CreateFromMaterial(_Material);
    _Mesh = MeshFactory::GetCube(100.0f, true);
  }

  void Skybox::SetTexture(Texture* texture) {
    _MaterialInstance->GetParameters().CubeTexture = texture->ToCubeMap(1024, L"Skybox Cubemap");
  }

  void Skybox::Draw(CommandList* commandList) {
    _Material->ApplySkyboxParameters(commandList, _MaterialInstance->GetParameters());
    _Mesh->Draw(commandList);
  }

  void SkyboxMaterial::ApplySkyboxParameters(CommandList* commandList, SkyboxMaterialParameters& params) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = params.CubeTexture->GetFormat();
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = (UINT)-1; // Use all mips.

    // TODO: Need a better way to bind a cubemap.
    commandList->SetShaderResourceViewTexture(
      1, 0, params.CubeTexture.get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 0, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, &srvDesc
    );

  }

  void SkyboxMaterial::CreateProgram() {
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
    _ProgramSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    _Program->SetRootSignature(_ProgramSignature.get());
    _Program->SetInputLayout(1, inputLayout);
    _Program->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _Program->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _Program->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));

    D3D12_RT_FORMAT_ARRAY rtFormat = {}; //TODO Woher kriegen wenn nicht stehlen?
    rtFormat.NumRenderTargets = 1;
    rtFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT dsFormat = DXGI_FORMAT_D32_FLOAT;
    _Program->SetRenderTargetFormats(rtFormat, dsFormat);

    _Program->Create();
  }

}


