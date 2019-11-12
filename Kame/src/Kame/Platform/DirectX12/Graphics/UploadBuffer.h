#pragma once

#include "Kame/Memory/Memory.h"
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include <deque>

#define DEFAULT_ALIGN 256

namespace Kame {

  class UploadBuffer { // Mini-Engine : LinearAllocator

  public: // Functions

    struct Allocation {
      void* Cpu;
      D3D12_GPU_VIRTUAL_ADDRESS Gpu;
    };

    explicit UploadBuffer(size_t pageSize = _2MB);

    size_t GetPageSize() const { return _PageSize; }

    Allocation Allocate(size_t sizeInBytes, size_t alignment = DEFAULT_ALIGN);
    void Reset();

  private: // Fields

    struct Page {
      Page(size_t sizeInBytes);
      ~Page();

      bool HasSpace(size_t sizeInBytes, size_t alignment) const;

      Allocation Allocate(size_t sizeInBytes, size_t alignment);

      void Reset();

    private: // Fields

      Microsoft::WRL::ComPtr < ID3D12Resource> _D3D12Resource;

      void* _CpuPtr;
      D3D12_GPU_VIRTUAL_ADDRESS _GpuPtr;

      size_t _PageSize;
      size_t _Offset;

    };

    using PagePool = std::deque<std::shared_ptr<Page>>;

    PagePool _PagePool;
    PagePool _AvailablePages;
    std::shared_ptr<Page> _CurrentPage;
    size_t _PageSize;

  private: // Functions

    std::shared_ptr<Page> RequestPage();
  };

}
