#pragma once

#include "GpuResource.h"

namespace Kame {

  class GpuBuffer : public GpuResource {

  public:
    GpuBuffer(const std::wstring& name = L"");
    GpuBuffer(
      const D3D12_RESOURCE_DESC& resDesc,
      size_t numElements, size_t elementSize,
      const std::wstring& name = L""
    );

    /**
     * Create the views for the buffer resource.
     * Used by the CommandList when setting the buffer contents.
     */
    virtual void CreateViews(size_t numElements, size_t elementSize) = 0;

  };

}