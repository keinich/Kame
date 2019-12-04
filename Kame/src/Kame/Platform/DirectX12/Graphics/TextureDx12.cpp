#include "kmpch.h"

#include "TextureDx12.h"

#include "DX12Core.h"
#include "Helpers.h"
#include "ResourceStateTracker.h"
#include "CommandListDx12.h"
#include "CommandQueue.h"

namespace Kame {

  TextureDx12::TextureDx12(TextureUsage textureUsage, const std::wstring& name)
    : GpuResourceDx12(name)
    , m_TextureUsage(textureUsage) {}

  TextureDx12::TextureDx12(const D3D12_RESOURCE_DESC& resourceDesc,
    const D3D12_CLEAR_VALUE* clearValue,
    TextureUsage textureUsage,
    const std::wstring& name)
    : GpuResourceDx12(resourceDesc, clearValue, name)
    , m_TextureUsage(textureUsage) {
    CreateViews();
  }

  TextureDx12::TextureDx12(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
    TextureUsage textureUsage,
    const std::wstring& name)
    : GpuResourceDx12(resource, name)
    , m_TextureUsage(textureUsage) {
    CreateViews();
  }

  TextureDx12::TextureDx12(const TextureDx12& copy)
    : GpuResourceDx12(copy) {
    CreateViews();
  }

  TextureDx12::TextureDx12(TextureDx12&& copy)
    : GpuResourceDx12(copy) {
    CreateViews();
  }

  TextureDx12& TextureDx12::operator=(const TextureDx12& other) {
    GpuResourceDx12::operator=(other);

    CreateViews();

    return *this;
  }
  TextureDx12& TextureDx12::operator=(TextureDx12&& other) {
    GpuResourceDx12::operator=(other);

    CreateViews();

    return *this;
  }

  TextureDx12::~TextureDx12() {}

  void TextureDx12::LoadFromFile(const std::wstring& fileName, TextureUsage textureUsage) {
    Reference<CommandListDx12> commandList = DX12Core::Get().GetCommandQueue(
      D3D12_COMMAND_LIST_TYPE_COPY
    )->GetCommandList();
    commandList->LoadTextureFromFile(*this, fileName, textureUsage);
    uint64_t fenceValue = DX12Core::Get().GetCommandQueue(
      D3D12_COMMAND_LIST_TYPE_COPY
    )->ExecuteCommandList(commandList);
    DX12Core::Get().GetCommandQueue(
      D3D12_COMMAND_LIST_TYPE_COPY
    )->WaitForFenceValue(fenceValue);
  }

  Reference<Texture> TextureDx12::ToCubeMap(UINT64 width, std::wstring name) {
    auto cubemapDesc = this->GetD3D12ResourceDesc();
    cubemapDesc.Width = cubemapDesc.Height = width;
    cubemapDesc.DepthOrArraySize = 6;
    cubemapDesc.MipLevels = 0;

    auto ret = CreateReference<TextureDx12>(cubemapDesc, nullptr, TextureUsage::Albedo, L"Grace Cathedral Cubemap");
    // Convert the 2D panorama to a 3D cubemap.
    auto commandList = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->GetCommandList();
    commandList->PanoToCubemap(*ret, *this);
    uint64_t fenceValue = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandList(commandList);
    DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->WaitForFenceValue(fenceValue);

    return ret;
  }

  void TextureDx12::Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize) {
    // Resource can't be resized if it was never created in the first place.
    if (m_d3d12Resource) {
      ResourceStateTracker::RemoveGlobalResourceState(m_d3d12Resource.Get());

      CD3DX12_RESOURCE_DESC resDesc(m_d3d12Resource->GetDesc());

      resDesc.Width = std::max(width, 1u);
      resDesc.Height = std::max(height, 1u);
      resDesc.DepthOrArraySize = depthOrArraySize;

      auto device = DX12Core::Get().GetDevice();

      ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_COMMON,
        m_d3d12ClearValue.get(),
        IID_PPV_ARGS(&m_d3d12Resource)
      ));

      // Retain the name of the resource if one was already specified.
      m_d3d12Resource->SetName(m_ResourceName.c_str());

      ResourceStateTracker::AddGlobalResourceState(m_d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

      CreateViews();
    }
  }

  // Get a UAV description that matches the resource description.
  D3D12_UNORDERED_ACCESS_VIEW_DESC GetUAVDesc(const D3D12_RESOURCE_DESC& resDesc, UINT mipSlice, UINT arraySlice = 0, UINT planeSlice = 0) {
    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = resDesc.Format;

    switch (resDesc.Dimension) {
    case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
      if (resDesc.DepthOrArraySize > 1) {
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
        uavDesc.Texture1DArray.ArraySize = resDesc.DepthOrArraySize - arraySlice;
        uavDesc.Texture1DArray.FirstArraySlice = arraySlice;
        uavDesc.Texture1DArray.MipSlice = mipSlice;
      }
      else {
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
        uavDesc.Texture1D.MipSlice = mipSlice;
      }
      break;
    case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
      if (resDesc.DepthOrArraySize > 1) {
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        uavDesc.Texture2DArray.ArraySize = resDesc.DepthOrArraySize - arraySlice;
        uavDesc.Texture2DArray.FirstArraySlice = arraySlice;
        uavDesc.Texture2DArray.PlaneSlice = planeSlice;
        uavDesc.Texture2DArray.MipSlice = mipSlice;
      }
      else {
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uavDesc.Texture2D.PlaneSlice = planeSlice;
        uavDesc.Texture2D.MipSlice = mipSlice;
      }
      break;
    case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
      uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
      uavDesc.Texture3D.WSize = resDesc.DepthOrArraySize - arraySlice;
      uavDesc.Texture3D.FirstWSlice = arraySlice;
      uavDesc.Texture3D.MipSlice = mipSlice;
      break;
    default:
      throw std::exception("Invalid resource dimension.");
    }

    return uavDesc;
  }

  void TextureDx12::CreateViews() {
    if (m_d3d12Resource) {
      auto& app = DX12Core::Get();
      auto device = app.GetDevice();

      CD3DX12_RESOURCE_DESC desc(m_d3d12Resource->GetDesc());

      if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 &&
        CheckRTVSupport()) {
        m_RenderTargetView = app.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        device->CreateRenderTargetView(m_d3d12Resource.Get(), nullptr, m_RenderTargetView.GetDescriptorHandle());
      }
      if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 &&
        CheckDSVSupport()) {
        m_DepthStencilView = app.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        device->CreateDepthStencilView(m_d3d12Resource.Get(), nullptr, m_DepthStencilView.GetDescriptorHandle());
      }
    }

    std::lock_guard<std::mutex> lock(m_ShaderResourceViewsMutex);
    std::lock_guard<std::mutex> guard(m_UnorderedAccessViewsMutex);

    // SRVs and UAVs will be created as needed.
    m_ShaderResourceViews.clear();
    m_UnorderedAccessViews.clear();
  }

  DescriptorAllocation TextureDx12::CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    auto& app = DX12Core::Get();
    auto device = app.GetDevice();
    auto srv = app.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    device->CreateShaderResourceView(m_d3d12Resource.Get(), srvDesc, srv.GetDescriptorHandle());

    return srv;
  }

  DescriptorAllocation TextureDx12::CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    auto& app = DX12Core::Get();
    auto device = app.GetDevice();
    auto uav = app.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    device->CreateUnorderedAccessView(m_d3d12Resource.Get(), nullptr, uavDesc, uav.GetDescriptorHandle());

    return uav;
  }


  D3D12_CPU_DESCRIPTOR_HANDLE TextureDx12::GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const {
    std::size_t hash = 0;
    if (srvDesc) {
      hash = std::hash<D3D12_SHADER_RESOURCE_VIEW_DESC>{}(*srvDesc);
    }

    std::lock_guard<std::mutex> lock(m_ShaderResourceViewsMutex);

    auto iter = m_ShaderResourceViews.find(hash);
    if (iter == m_ShaderResourceViews.end()) {
      auto srv = CreateShaderResourceView(srvDesc);
      iter = m_ShaderResourceViews.insert({ hash, std::move(srv) }).first;
    }

    return iter->second.GetDescriptorHandle();
  }

  D3D12_CPU_DESCRIPTOR_HANDLE TextureDx12::GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const {
    std::size_t hash = 0;
    if (uavDesc) {
      hash = std::hash<D3D12_UNORDERED_ACCESS_VIEW_DESC>{}(*uavDesc);
    }

    std::lock_guard<std::mutex> guard(m_UnorderedAccessViewsMutex);

    auto iter = m_UnorderedAccessViews.find(hash);
    if (iter == m_UnorderedAccessViews.end()) {
      auto uav = CreateUnorderedAccessView(uavDesc);
      iter = m_UnorderedAccessViews.insert({ hash, std::move(uav) }).first;
    }

    return iter->second.GetDescriptorHandle();
  }

  D3D12_CPU_DESCRIPTOR_HANDLE TextureDx12::GetRenderTargetView() const {
    return m_RenderTargetView.GetDescriptorHandle();
  }

  D3D12_CPU_DESCRIPTOR_HANDLE TextureDx12::GetDepthStencilView() const {
    return m_DepthStencilView.GetDescriptorHandle();
  }

  bool TextureDx12::IsUAVCompatibleFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SINT:
      return true;
    default:
      return false;
    }
  }

  bool TextureDx12::IsSRGBFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
      return true;
    default:
      return false;
    }
  }

  bool TextureDx12::IsBGRFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
      return true;
    default:
      return false;
    }

  }

  bool TextureDx12::IsDepthFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D16_UNORM:
      return true;
    default:
      return false;
    }
  }

  DXGI_FORMAT TextureDx12::GetTypelessFormat(DXGI_FORMAT format) {
    DXGI_FORMAT typelessFormat = format;

    switch (format) {
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
      typelessFormat = DXGI_FORMAT_R32G32B32A32_TYPELESS;
      break;
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
      typelessFormat = DXGI_FORMAT_R32G32B32_TYPELESS;
      break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
      typelessFormat = DXGI_FORMAT_R16G16B16A16_TYPELESS;
      break;
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
      typelessFormat = DXGI_FORMAT_R32G32_TYPELESS;
      break;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
      typelessFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
      break;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
      typelessFormat = DXGI_FORMAT_R10G10B10A2_TYPELESS;
      break;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
      typelessFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
      break;
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
      typelessFormat = DXGI_FORMAT_R16G16_TYPELESS;
      break;
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
      typelessFormat = DXGI_FORMAT_R32_TYPELESS;
      break;
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
      typelessFormat = DXGI_FORMAT_R8G8_TYPELESS;
      break;
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
      typelessFormat = DXGI_FORMAT_R16_TYPELESS;
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
      typelessFormat = DXGI_FORMAT_R8_TYPELESS;
      break;
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_BC1_TYPELESS;
      break;
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_BC2_TYPELESS;
      break;
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_BC3_TYPELESS;
      break;
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
      typelessFormat = DXGI_FORMAT_BC4_TYPELESS;
      break;
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
      typelessFormat = DXGI_FORMAT_BC5_TYPELESS;
      break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
      break;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
      break;
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
      typelessFormat = DXGI_FORMAT_BC6H_TYPELESS;
      break;
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
      typelessFormat = DXGI_FORMAT_BC7_TYPELESS;
      break;
    }

    return typelessFormat;
  }

  DXGI_FORMAT TextureDx12::GetUAVCompatableFormat(DXGI_FORMAT format) {
    DXGI_FORMAT uavFormat = format;

    switch (format) {
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
      uavFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
      break;
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
      uavFormat = DXGI_FORMAT_R32_FLOAT;
      break;
    }

    return uavFormat;
  }

}
