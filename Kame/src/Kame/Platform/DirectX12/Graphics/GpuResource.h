#pragma once

#include "kmpch.h"

namespace Kame {
  
class GpuResource {

  friend class CommandContext;
  friend class GraphicsContext;
  friend class ComputeContext;

public:
  GpuResource(const std::wstring& name = L"");
  GpuResource(
    const D3D12_RESOURCE_DESC& resourceDesc,
    const D3D12_CLEAR_VALUE* clearValue = nullptr,
    const std::wstring& name = L""
  );
  GpuResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, const std::wstring& name = L"");
  GpuResource(const GpuResource& copy);
  GpuResource(GpuResource&& copy);

  GpuResource& operator=(const GpuResource& other);
  GpuResource& operator=(GpuResource&& other);

  virtual ~GpuResource() {};
  
  bool IsValid() const {
    return (_Resource1 != nullptr);
  }

  ComPtr<ID3D12Resource> GetD3D12Resource() const { return _Resource1; }

  D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const {
    D3D12_RESOURCE_DESC resDesc = {};
    if (_Resource1) {
      resDesc = _Resource1->GetDesc();
    }

    return resDesc;
  }

  // Replace the D3D12 resource
   // Should only be called by the CommandList.
  virtual void SetD3D12Resource(
    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource,
    const D3D12_CLEAR_VALUE* clearValue = nullptr
  );

  /**
   * Get the SRV for a resource.
   *
   * @param srvDesc The description of the SRV to return. The default is nullptr
   * which returns the default SRV for the resource (the SRV that is created when no
   * description is provided.
   */
  virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const = 0;

  /**
   * Get the UAV for a (sub)resource.
   *
   * @param uavDesc The description of the UAV to return.
   */
  virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const = 0;

  void SetName(const std::wstring& name);

  void Destroy();

protected:
  //ID3D12Resource* _Resource; // TODO in Mini-Engine this is a ComPtr -> why?
  ComPtr<ID3D12Resource> _Resource1; // TODO in Mini-Engine this is a ComPtr -> why?
  D3D12_RESOURCE_STATES _UsageState;
  std::unique_ptr<D3D12_CLEAR_VALUE> _D3D12ClearValue;
  std::wstring _ResourceName;
};

}

