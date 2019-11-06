#pragma once

#include "kmpch.h"

#include <cstdint>

#include "ColorBuffer.h"
#include "DescriptorAllocator.h"

namespace Kame {

  class CommandManager;
  class ContextManager;

  extern DescriptorAllocator g_DescriptorAllocators[];

  class KAME_API DX12Core {

  private:

    static DX12Core* _Instance;
    DX12Core();

  public:
    ~DX12Core();

    static DX12Core* Get() { return _Instance; }

    void Init();
    void ShutDown();

    void Render();

    void Resize(uint32_t width, uint32_t height);
    void SetFullscreen(bool fullscreen);
    void SwitchFullscreen();

    bool _IsInitialized = false;

    inline static ContextManager* GetContextManager() { return _Instance->_ContextManager; }
    inline static CommandManager* GetCommandManager() { return _Instance->_CommandManager; }
    inline static GlobalDescriptorAllocator* GetGlobalDescriptorAllocator() { return _Instance->_GlobalDescriptorAllocator; }
    inline static ID3D12Device* GetDevice() { return _Instance->_Device.Get(); }

    inline static D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count = 1) {
      return g_DescriptorAllocators[type].Allocate(count);
    }

    inline const bool IsTogglingFullscreen() const { return _TogglingFullscreen; }
    inline const void FinishTogglingFullscreen() { _TogglingFullscreen = false; }

  private: // Functions

    void EnableDebugLayer();
    ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp);
    ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIAdapter4> adapter);
    ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
    bool CheckTearingSupport();
    ComPtr<IDXGISwapChain4> CreateSwapChain(
      HWND hWnd,
      ID3D12CommandQueue* commandQueue,
      uint32_t width, uint32_t height,
      uint32_t bufferCount
    );
    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);
    void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain);
    ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
    ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
      ComPtr<ID3D12Device2> device,
      ComPtr<ID3D12CommandAllocator> commandAllocator,
      D3D12_COMMAND_LIST_TYPE type
    );
    ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device);
    HANDLE CreateEventHandle();
    uint64_t Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue);
    void WaitForFenceValue(
      ComPtr<ID3D12Fence> fence,
      uint64_t fenceValue,
      HANDLE fenceEvent,
      std::chrono::milliseconds duration = std::chrono::milliseconds::max()
    );
    void Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent);

    void UpdateBufferResource(
      ID3D12GraphicsCommandList* commandList,
      ID3D12Resource** destinationResource,
      ID3D12Resource** intermediateResource,
      size_t numElements, size_t elementSize,
      const void* buffeDatar,
      D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
    );

    bool LoadContent();

    void ResizeDepthBuffer(int width, int height);

    void GameUpdate();

  private: // Fields
    static const uint8_t c_NumFrames = 3;
    bool _UseWarp = false;

    uint32_t _ClientWidth = 1280;
    uint32_t _ClientHeight = 720;


    HWND g_hWnd;
    RECT _WindowRect;

    // DX12 Core Object
    ComPtr<ID3D12Device2> _Device;
    CommandManager* _CommandManager;
    ContextManager* _ContextManager;
    GlobalDescriptorAllocator* _GlobalDescriptorAllocator;
    ComPtr<IDXGISwapChain4> g_SwapChain;
    //ComPtr<ID3D12Resource> g_BackBuffers[c_NumFrames];
    ColorBuffer g_BackBuffers1[c_NumFrames];
    //ComPtr<ID3D12GraphicsCommandList> g_CommandList;
    //ComPtr<ID3D12CommandAllocator> g_CommandAllocator[c_NumFrames];
    //ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
    //UINT g_RTVDescriptorSize;
    UINT g_CurrentBackBufferIndex;

    uint64_t g_FrameFenceValues[c_NumFrames] = {};

    CD3DX12_RECT _ScissorRect;
    CD3DX12_VIEWPORT _Viewport;

    bool _ContentLoaded;

    // Mesh Stuff
    D3D12_VERTEX_BUFFER_VIEW _VertexBufferView;
    ComPtr<ID3D12Resource> _VertexBuffer;
    D3D12_INDEX_BUFFER_VIEW _IndexBufferView;
    ComPtr<ID3D12Resource> _IndexBuffer;

    // Depth Stuff
    ComPtr<ID3D12Resource> _DepthBuffer;
    ComPtr<ID3D12DescriptorHeap> _DsvHeap;

    ComPtr<ID3D12RootSignature> _RootSignature;
    ComPtr<ID3D12PipelineState> _PipelineState;

    bool g_VSync = true;
    bool _TearingSupported = false;
    bool _Fullscreen = false;
    bool _TogglingFullscreen = false;
    
    // Camera Stuff
    float _FoV;

    DirectX::XMMATRIX _ModelMatrix;
    DirectX::XMMATRIX _ViewMatrix;
    DirectX::XMMATRIX _ProjectionMatrix;

  };

}