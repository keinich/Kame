#include "kmpch.h"
#include "GpuResource.h"

#include "DX12Core.h"
#include "ResourceStateTracker.h"

namespace Kame {
  
  GpuResource::GpuResource(const std::wstring& name)
    : _ResourceName(name) {}

  GpuResource::GpuResource(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue, const std::wstring& name) {
    auto device = DX12Core::Get()->GetDevice();

    if (clearValue) {
      _D3D12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }

    ThrowIfFailed(device->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
      D3D12_HEAP_FLAG_NONE,
      &resourceDesc,
      D3D12_RESOURCE_STATE_COMMON,
      _D3D12ClearValue.get(),
      IID_PPV_ARGS(&_Resource1)
    ));

    ResourceStateTracker::AddGlobalResourceState(_Resource1.Get(), D3D12_RESOURCE_STATE_COMMON);

    SetName(name);
  }

  GpuResource::GpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name)
    : _Resource1(resource) {
    SetName(name);
  }

  GpuResource::GpuResource(const GpuResource& copy)
    : _Resource1(copy._Resource1)
    , _ResourceName(copy._ResourceName)
    , _D3D12ClearValue(std::make_unique<D3D12_CLEAR_VALUE>(*copy._D3D12ClearValue)) {
    int i = 3;
  }

  GpuResource::GpuResource(GpuResource&& copy)
    : _Resource1(std::move(copy._Resource1))
    , _ResourceName(std::move(copy._ResourceName))
    , _D3D12ClearValue(std::move(copy._D3D12ClearValue)) {}

  GpuResource& GpuResource::operator=(const GpuResource& other) {
    if (this != &other) {
      _Resource1 = other._Resource1;
      _ResourceName = other._ResourceName;
      if (other._D3D12ClearValue) {
        _D3D12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*other._D3D12ClearValue);
      }
    }

    return *this;
  }

  GpuResource& GpuResource::operator=(GpuResource&& other) {
    if (this != &other) {
      _Resource1 = other._Resource1;
      _ResourceName = other._ResourceName;
      _D3D12ClearValue = std::move(other._D3D12ClearValue);

      other._Resource1.Reset();
      other._ResourceName.clear();
    }

    return *this;
  }

  void GpuResource::SetD3D12Resource(Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource, const D3D12_CLEAR_VALUE* clearValue) {
    _Resource1 = d3d12Resource;
    if (_D3D12ClearValue) {
      _D3D12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }
    else {
      _D3D12ClearValue.reset();
    }
    SetName(_ResourceName);
  }

  void GpuResource::SetName(const std::wstring& name) {
    _ResourceName = name;
    if (_Resource1 && !_ResourceName.empty()) {
      _Resource1->SetName(_ResourceName.c_str());
    }
  }

  void GpuResource::Destroy() {
    _Resource1 = nullptr;
  }

}

