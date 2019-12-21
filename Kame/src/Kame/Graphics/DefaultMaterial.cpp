#include "kmpch.h"
#include "DefaultMaterial.h"

#include <Kame/Core/DebugUtilities.h>
#include <Kame/Graphics/RenderApi/CommandList.h>
#include <Kame/Graphics/RenderApi/RenderProgramSignature.h>
#include <Kame/Graphics/Mesh.h>
#include <Kame/Graphics/Renderer3D.h>
#include <Kame\Graphics\MaterialManager.h>

namespace Kame {

  std::vector<Texture*> DefaultMaterialParameters::_Textures;

  void DefaultMaterialParameters::SetDiffuseTexture(Texture* texture) {
    for (UINT i = 0; i < _Textures.size(); ++i) {
      if (_Textures[i] == texture) {
        TextureIndexStatic = i;
        TextureIndexForShader = i;
        return;
      }
    }
    _Textures.push_back(texture);
    TextureIndexStatic = _Textures.size() - 1;
    TextureIndexForShader = _Textures.size() - 1;
    DefaultMaterial* mat = MaterialManager::GetMaterial<DefaultMaterial>();
    mat->CreateProgram();
  }

  void DefaultMaterial::ApplyParameters(CommandList* commandList, std::vector<DefaultMaterialParameters>& params) {
    for (UINT i = 0; i < DefaultMaterialParameters::_Textures.size(); ++i) {
      Texture* texture = DefaultMaterialParameters::_Textures[i];
      commandList->SetShaderResourceViewTexture(
        MaterialRootParameters::Textures,
        i,
        texture,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
      );
    }
    commandList->SetGraphicsDynamicStructuredBuffer(MaterialRootParameters::MaterialParameters, params);
  }

  void DefaultMaterial::ApplyParameters(
    CommandList* commandList, 
    std::vector<MaterialInstanceBase*>& materialInstances
  ) {
    std::vector<DefaultMaterialParameters> params;
    std::map<uint32_t, uint32_t> shaderIndexByStaticIndex;
    uint32_t currentShaderIndex = 0;
    for (MaterialInstanceBase* matInstance : materialInstances) {
      DefaultMaterialParameters& parameters = *reinterpret_cast<DefaultMaterialParameters*>(matInstance->GetParameters1());
      auto shaderIndexIt = shaderIndexByStaticIndex.find(parameters.TextureIndexStatic);
      if (shaderIndexIt == shaderIndexByStaticIndex.end()) {
        parameters.TextureIndexForShader = currentShaderIndex;
        currentShaderIndex++;
        shaderIndexByStaticIndex.insert(std::map<uint32_t, uint32_t>::value_type(parameters.TextureIndexStatic, parameters.TextureIndexForShader));
        Texture* texture = DefaultMaterialParameters::_Textures[parameters.TextureIndexStatic];
        commandList->SetShaderResourceViewTexture(
          MaterialRootParameters::Textures,
          parameters.TextureIndexForShader,
          texture,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
      }
      else {
        parameters.TextureIndexForShader = shaderIndexIt->second;
      }

      params.push_back(parameters);
    }

    commandList->SetGraphicsDynamicStructuredBuffer(MaterialRootParameters::MaterialParameters, params);

  }

  void DefaultMaterial::CreateProgram() {

    //TODO Have Base RootSignature and Base RootParameters
    //Here should only be declared the extensions of thos Bases

    ComPtr<ID3DBlob> vs;
    ComPtr<ID3DBlob> ps;
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDR_I_VS.cso", &vs));
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Sandbox\\HDR_I_PS.cso", &ps));

    // Allow input layout and deny unnecessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    UINT numberOfTextures = Kame::DefaultMaterialParameters::_Textures.size();
    if (numberOfTextures == 0)
      numberOfTextures = 1;
    CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numberOfTextures, 2);

    CD3DX12_ROOT_PARAMETER1 rootParameters[MaterialRootParameters::NumRootParameters];
    rootParameters[MaterialRootParameters::MatricesCB].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[MaterialRootParameters::MaterialCB].InitAsConstantBufferView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[MaterialRootParameters::LightPropertiesCB].InitAsConstants(sizeof(LightProperties) / 4, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[MaterialRootParameters::InstanceDataSlot].InitAsShaderResourceView(0, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[MaterialRootParameters::PointLights].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[MaterialRootParameters::SpotLights].InitAsShaderResourceView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[MaterialRootParameters::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[MaterialRootParameters::MaterialParameters].InitAsShaderResourceView(1, 1, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_PIXEL);

    CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
    CD3DX12_STATIC_SAMPLER_DESC anisotropicSampler(0, D3D12_FILTER_ANISOTROPIC);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(MaterialRootParameters::NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

    //m_HDRRootSignature->SetDescription(rootSignatureDescription.Desc_1_1, featureData.HighestVersion);
    _ProgramSignature->SetDescription(rootSignatureDescription.Desc_1_1);

    _Program->SetRootSignature(_ProgramSignature.get());
    _Program->SetInputLayout(VertexPositionNormalTexture::InputElementCount, VertexPositionNormalTexture::InputElements);
    _Program->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    _Program->SetVertexShader(CD3DX12_SHADER_BYTECODE(vs.Get()));
    _Program->SetPixelShader(CD3DX12_SHADER_BYTECODE(ps.Get()));
    D3D12_RT_FORMAT_ARRAY rtFormat = {}; //TODO Woher kriegen wenn nicht stehlen?
    rtFormat.NumRenderTargets = 1;
    rtFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
    DXGI_FORMAT dsFormat = DXGI_FORMAT_D32_FLOAT;
    _Program->SetRenderTargetFormats(
      rtFormat,
      dsFormat
    );

    _Program->Create();
  }

}