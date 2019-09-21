#pragma once

#include "kmpch.h"

#include "CommandManager.h"

#include <cstdint>

namespace Kame {

  class KAME_API DxTutorial {

  public:
    DxTutorial();
    ~DxTutorial();

    void Init();
    void ShutDown();

    void Render();

    void Resize(uint32_t width, uint32_t height);
    void SetFullscreen(bool fullscreen);
    void SwitchFullscreen();
       
    // TODO : remove this
    static DxTutorial* _Instance;
    bool _IsInitialized = false;

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
    void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device, ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap);
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

  private: // Fields
    static const uint8_t c_NumFrames = 3;
    bool _UseWarp = false;

    uint32_t _ClientWidth = 1280;
    uint32_t _ClientHeight = 720;


    HWND g_hWnd;
    RECT _WindowRect;

    // DX12 Core Object
    ComPtr<ID3D12Device2> g_Device;
    //ComPtr<ID3D12CommandQueue> g_CommandQueue;
    CommandManager* _CommandManager;
    ComPtr<IDXGISwapChain4> g_SwapChain;
    ComPtr<ID3D12Resource> g_BackBuffers[c_NumFrames];
    ComPtr<ID3D12GraphicsCommandList> g_CommandList;
    ComPtr<ID3D12CommandAllocator> g_CommandAllocator[c_NumFrames];
    ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
    UINT g_RTVDescriptorSize;
    UINT g_CurrentBackBufferIndex;

    // DX12 Sync Objects
    //ComPtr<ID3D12Fence> _Fence;
    //uint64_t g_FenceValue = 0;
    uint64_t g_FrameFenceValues[c_NumFrames] = {};
    //HANDLE g_FenceEvent;

    bool g_VSync = true;
    bool _TearingSupported = false;
    bool _Fullscreen = false;

  };  

}