#include "kmpch.h"

#include "GpuResourceDx12.h"

#include "DX12Core.h"
#include "ResourceStateTracker.h"
#include <Kame/Core/DebugUtilities.h>

namespace Kame {

  GpuResourceDx12::GpuResourceDx12(const std::wstring& name)
    : m_ResourceName(name)
    , m_FormatSupport({}) {}

  GpuResourceDx12::GpuResourceDx12(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, const std::wstring& name) {
    if (clearValue) {
      m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }

    auto device = DX12Core::Get().GetDevice();

    ThrowIfFailed(device->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
      D3D12_HEAP_FLAG_NONE,
      &resourceDesc,
      D3D12_RESOURCE_STATE_COMMON,
      m_d3d12ClearValue.get(),
      IID_PPV_ARGS(&m_d3d12Resource)
    ));

    ResourceStateTracker::AddGlobalResourceState(m_d3d12Resource.Get(), D3D12_RESOURCE_STATE_COMMON);

    CheckFeatureSupport();
    SetName(name);
  }

  GpuResourceDx12::GpuResourceDx12(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name)
    : m_d3d12Resource(resource)
    , m_FormatSupport({}) {
    CheckFeatureSupport();
    SetName(name);
  }

  GpuResourceDx12::GpuResourceDx12(const GpuResourceDx12& copy)
    : m_d3d12Resource(copy.m_d3d12Resource)
    , m_FormatSupport(copy.m_FormatSupport)
    , m_ResourceName(copy.m_ResourceName)
    , m_d3d12ClearValue(std::make_unique<D3D12_CLEAR_VALUE>(*copy.m_d3d12ClearValue)) {}

  GpuResourceDx12::GpuResourceDx12(GpuResourceDx12&& copy)
    : m_d3d12Resource(std::move(copy.m_d3d12Resource))
    , m_FormatSupport(copy.m_FormatSupport)
    , m_ResourceName(std::move(copy.m_ResourceName))
    , m_d3d12ClearValue(std::move(copy.m_d3d12ClearValue)) {}

  GpuResourceDx12& GpuResourceDx12::operator=(const GpuResourceDx12& other) {
    if (this != &other) {
      m_d3d12Resource = other.m_d3d12Resource;
      m_FormatSupport = other.m_FormatSupport;
      m_ResourceName = other.m_ResourceName;
      if (other.m_d3d12ClearValue) {
        m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*other.m_d3d12ClearValue);
      }
    }

    return *this;
  }

  GpuResourceDx12& GpuResourceDx12::operator=(GpuResourceDx12&& other) noexcept {
    if (this != &other) {
      m_d3d12Resource = std::move(other.m_d3d12Resource);
      m_FormatSupport = other.m_FormatSupport;
      m_ResourceName = std::move(other.m_ResourceName);
      m_d3d12ClearValue = std::move(other.m_d3d12ClearValue);

      other.Reset();
    }

    return *this;
  }


  GpuResourceDx12::~GpuResourceDx12() {}

  void GpuResourceDx12::SetD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, const D3D12_CLEAR_VALUE* clearValue) {
    m_d3d12Resource = d3d12Resource;
    if (m_d3d12ClearValue) {
      m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }
    else {
      m_d3d12ClearValue.reset();
    }
    CheckFeatureSupport();
    SetName(m_ResourceName);
  }

  void GpuResourceDx12::SetName(const std::wstring& name) {
    m_ResourceName = name;
    if (m_d3d12Resource && !m_ResourceName.empty()) {
      m_d3d12Resource->SetName(m_ResourceName.c_str());
    }
  }

  void GpuResourceDx12::Reset() {
    m_d3d12Resource.Reset();
    m_FormatSupport = {};
    m_d3d12ClearValue.reset();
    m_ResourceName.clear();
  }

  bool GpuResourceDx12::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const {
    return (m_FormatSupport.Support1 & formatSupport) != 0;
  }

  bool GpuResourceDx12::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const {
    return (m_FormatSupport.Support2 & formatSupport) != 0;
  }

  void GpuResourceDx12::CheckFeatureSupport() {
    if (m_d3d12Resource) {
      auto desc = m_d3d12Resource->GetDesc();
      auto device = DX12Core::Get().GetDevice();

      m_FormatSupport.Format = desc.Format;
      ThrowIfFailed(device->CheckFeatureSupport(
        D3D12_FEATURE_FORMAT_SUPPORT,
        &m_FormatSupport,
        sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT)));
    }
    else {
      m_FormatSupport = {};
    }
  }

}