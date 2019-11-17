#pragma once

#include "kmpch.h"

#include <cstdint>

#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "DescriptorAllocator.h"
#include "DescriptorAllocator_3dgep.h"
#include "GraphicsPipelineState.h"
#include "RootSignature.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "CommandQueue.h"

namespace Kame {

  class CommandManager;
  class ContextManager;
  class PipelineStateManager;
  class DescriptorAllocator_3dgep;

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

    //inline static ContextManager* GetContextManager() { return _Instance->_ContextManager; }
    //inline static CommandManager* GetCommandManager() { return _Instance->_CommandManager; }
    std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
    inline static GlobalDescriptorAllocator* GetGlobalDescriptorAllocator() { return _Instance->_GlobalDescriptorAllocator; }
    inline static ID3D12Device2* GetDevice() { return _Instance->_Device.Get(); }
    inline static PipelineStateManager* GetPipelineStateManager() { return _Instance->_PipelineStateManager; }

    inline DescriptorAllocation AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT count = 1);

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

    void ReleaseStaleDescriptors(uint64_t finishedFrame);

    void Present(const Texture& texture = Texture());

  private: // Fields
    static const uint8_t c_NumFrames = 3;
    bool _UseWarp = false;

    uint32_t _ClientWidth = 1280;
    uint32_t _ClientHeight = 720;


    HWND g_hWnd;
    RECT _WindowRect;

    // DX12 Core Object
    ComPtr<ID3D12Device2> _Device;

    // Mini Engine Way for the Command Queues
    //CommandManager* _CommandManager;
    //ContextManager* _ContextManager;

    // 3dgep Way
    std::shared_ptr<CommandQueue> _DirectCommandQueue;
    std::shared_ptr<CommandQueue> _ComputeCommandQueue;
    std::shared_ptr<CommandQueue> _CopyCommandQueue;

    GlobalDescriptorAllocator* _GlobalDescriptorAllocator;
    ComPtr<IDXGISwapChain4> g_SwapChain;
    //ComPtr<ID3D12Resource> g_BackBuffers[c_NumFrames];
    Texture g_BackBuffers1[c_NumFrames];
    RenderTarget _HDRRenderTarget;

    //ComPtr<ID3D12GraphicsCommandList> g_CommandList;
    //ComPtr<ID3D12CommandAllocator> g_CommandAllocator[c_NumFrames];
    //ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
    //UINT g_RTVDescriptorSize;
    UINT g_CurrentBackBufferIndex;

    uint64_t g_FrameFenceValues[c_NumFrames] = {};
    uint64_t _FrameValues[c_NumFrames] = {};

    CD3DX12_RECT _ScissorRect;
    CD3DX12_VIEWPORT _Viewport;

    bool _ContentLoaded;

    // Mesh Stuff
    //D3D12_VERTEX_BUFFER_VIEW _VertexBufferView;
    //ComPtr<ID3D12Resource> _VertexBuffer;
    //D3D12_INDEX_BUFFER_VIEW _IndexBufferView;
    //ComPtr<ID3D12Resource> _IndexBuffer;
    VertexBuffer _VertexBuffer1;
    IndexBuffer _IndexBuffer1;

    // Depth Stuff
    //ComPtr<ID3D12Resource> _DepthBuffer;
    //ComPtr<ID3D12DescriptorHeap> _DsvHeap;

    Texture _SceneDepthBuffer;


    //ComPtr<ID3D12RootSignature> _RootSignature;
    RootSignature _RootSignature1;
    //ComPtr<ID3D12PipelineState> _PipelineState;
    GraphicsPipelineState _PipelineState1;
    GraphicsPipelineState _HDRPipelineState;
    PipelineStateManager* _PipelineStateManager;
    //GraphicsPipelineState _PipelineState2;

    std::unique_ptr<DescriptorAllocator_3dgep> _DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    bool g_VSync = true;
    bool _TearingSupported = false;
    bool _Fullscreen = false;
    bool _TogglingFullscreen = false;
    
    // Camera Stuff
    float _FoV;

    DirectX::XMMATRIX _ModelMatrix;
    DirectX::XMMATRIX _ViewMatrix;
    DirectX::XMMATRIX _ProjectionMatrix;

    uint64_t _FrameCount;
  };

}