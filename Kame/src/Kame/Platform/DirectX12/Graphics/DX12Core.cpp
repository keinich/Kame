#include "kmpch.h"
#include "DX12Core.h"
#include "Kame/Platform/Win32/Win32Window.h"
#include "Kame/Application.h"

#include "CommandManager.h"
#include "ContextManager.h"
#include "DescriptorAllocator.h"
#include "GraphicsContext.h"

#include "ColorBuffer.h"

namespace Kame {

  DX12Core* DX12Core::_Instance = new DX12Core();

  DescriptorAllocator g_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
      D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
      D3D12_DESCRIPTOR_HEAP_TYPE_DSV
  };

  DX12Core::DX12Core() {
    _CommandManager = new CommandManager();
    _ContextManager = new ContextManager();
    _GlobalDescriptorAllocator = GlobalDescriptorAllocator::Get();
    _WindowRect = RECT();
    g_CurrentBackBufferIndex = 0;
    
    // TODO Constructor verbessern
  }

  DX12Core::~DX12Core() {
    delete _CommandManager;
    delete _ContextManager;
    delete _GlobalDescriptorAllocator;
  }

  void DX12Core::Init() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    EnableDebugLayer();
    _TearingSupported = CheckTearingSupport();

    g_hWnd = (HWND)Application::Get().GetWindow().GetNativeWindow();
    ::GetWindowRect(g_hWnd, &_WindowRect);

    ComPtr<IDXGIAdapter4> dxgiAdapter4 = GetAdapter(_UseWarp);

    g_Device = CreateDevice(dxgiAdapter4);

    _CommandManager->Create(g_Device.Get());

    g_SwapChain = CreateSwapChain(g_hWnd, _CommandManager->GetCommandQueue(), _ClientWidth, _ClientHeight, c_NumFrames);

    g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

    g_RTVDescriptorHeap = CreateDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, c_NumFrames);
    g_RTVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap);

    for (int i = 0; i < c_NumFrames; ++i) {
      g_CommandAllocator[i] = CreateCommandAllocator(g_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }
    g_CommandList = CreateCommandList(g_Device, g_CommandAllocator[g_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

    _IsInitialized = true;

  }

  void DX12Core::EnableDebugLayer() {

#if defined KAME_DEBUG

    ComPtr<ID3D12Debug> debugInterface;
    HRESULT r = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    ThrowIfFailed(r);
    debugInterface->EnableDebugLayer();

#endif

  }

  ComPtr<IDXGIAdapter4> DX12Core::GetAdapter(bool useWarp) {
    ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;

#if defined KAME_DEBUG
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if (useWarp) {
      ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
      ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
    }
    else {
      SIZE_T maxDedicatedVideoMemory = 0;
      for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
        dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

        if (
          (dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
          SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
          dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory
          ) {
          maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
          ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
        }
      }
    }
    return dxgiAdapter4;
  }

  ComPtr<ID3D12Device2> DX12Core::CreateDevice(ComPtr<IDXGIAdapter4> adapter) {

    ComPtr<ID3D12Device2> d3d12Device2;
    ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));

#if defined KAME_DEBUG

    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(d3d12Device2.As(&infoQueue))) {
      infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
      infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
      infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    }

    D3D12_MESSAGE_SEVERITY severities[] = {
      D3D12_MESSAGE_SEVERITY_INFO
    };

    D3D12_MESSAGE_ID denyIds[] = {
      D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
      D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
      D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
    };

    D3D12_INFO_QUEUE_FILTER newFilter = {};
    newFilter.DenyList.NumSeverities = _countof(severities);
    newFilter.DenyList.pSeverityList = severities;
    newFilter.DenyList.NumIDs = _countof(denyIds);
    newFilter.DenyList.pIDList = denyIds;

    ThrowIfFailed(infoQueue->PushStorageFilter(&newFilter));

#endif

    return d3d12Device2;

  }

  ComPtr<ID3D12CommandQueue> DX12Core::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

    return d3d12CommandQueue;

  }

  bool DX12Core::CheckTearingSupport() {
    BOOL allowTearing = FALSE;

    // Rather than create the DXGI 1.5 factory interface directly, we create the
    // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
    // graphics debugging tools which will not support the 1.5 factory interface 
    // until a future update.
    ComPtr<IDXGIFactory4> factory4;
    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4)))) {
      ComPtr<IDXGIFactory5> factory5;
      if (SUCCEEDED(factory4.As(&factory5))) {
        if (FAILED(factory5->CheckFeatureSupport(
          DXGI_FEATURE_PRESENT_ALLOW_TEARING,
          &allowTearing, sizeof(allowTearing)))) {
          allowTearing = FALSE;
        }
      }
    }

    return allowTearing == TRUE;
  }

  ComPtr<IDXGISwapChain4> DX12Core::CreateSwapChain(
    HWND hWnd,
    ID3D12CommandQueue* commandQueue,
    uint32_t width, uint32_t height,
    uint32_t bufferCount
  ) {
    ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 1, 0 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(dxgiFactory4->CreateSwapChainForHwnd(
      commandQueue,
      hWnd,
      &swapChainDesc,
      nullptr,
      nullptr,
      &swapChain1));

    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

    return dxgiSwapChain4;

  }

  ComPtr<ID3D12DescriptorHeap> DX12Core::CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;

    ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
  }

  void DX12Core::UpdateRenderTargetViews(
    ComPtr<ID3D12Device2> device,
    ComPtr<IDXGISwapChain4> swapChain,
    ComPtr<ID3D12DescriptorHeap> descriptorHeap
  ) {
    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < c_NumFrames; ++i) {
      ComPtr<ID3D12Resource> backBuffer;
      ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

      g_BackBuffers1[i].CreateFromSwapChain(L"", backBuffer.Detach());

      //device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

      //g_BackBuffers[i] = backBuffer;

      rtvHandle.Offset(rtvDescriptorSize);

    }
  }

  ComPtr<ID3D12CommandAllocator> DX12Core::CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));

    return commandAllocator;
  }

  ComPtr<ID3D12GraphicsCommandList> DX12Core::CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

    ThrowIfFailed(commandList->Close());

    return commandList;
  }

  ComPtr<ID3D12Fence> DX12Core::CreateFence(ComPtr<ID3D12Device2> device) {
    ComPtr<ID3D12Fence> fence;

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    return fence;
  }

  HANDLE DX12Core::CreateEventHandle() {
    HANDLE fenceEvent;

    fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent && "Failed to create fence event.");

    return fenceEvent;
  }

  uint64_t DX12Core::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue) {
    uint64_t fenceValueForSignal = ++fenceValue;
    ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValue));

    return fenceValueForSignal;
  }

  void DX12Core::WaitForFenceValue(
    ComPtr<ID3D12Fence> fence,
    uint64_t fenceValue,
    HANDLE fenceEvent,
    std::chrono::milliseconds duration
  ) {
    if (fence->GetCompletedValue() < fenceValue) {
      ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
      ::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
    }
  }

  void DX12Core::Flush(
    ComPtr<ID3D12CommandQueue> commandQueue,
    ComPtr<ID3D12Fence> fence,
    uint64_t& fenceValue,
    HANDLE fenceEvent
  ) {
    uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
    WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
  }

  void DX12Core::Render() {
    auto commandAllocator = g_CommandAllocator[g_CurrentBackBufferIndex];
    //auto backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];
    auto backBuffer1 = g_BackBuffers1[g_CurrentBackBufferIndex];

    // CommandContext::Begin will Reset allocator and reset commandList
    GraphicsContext& myContext = GraphicsContext::Begin(L"Main Loop");

    // TODO zum rantasten
    g_CommandList = myContext.GetCommandList();
    commandAllocator = myContext.GetCurrentAllocator();

    //commandAllocator->Reset();
    //g_CommandList->Reset(commandAllocator.Get(), nullptr);

    // Clear the render target.
    {
      //myContext.TransitionResource();

      //// Transition is (new?) Helper-function. Its body is "manually" done in CommandContext::TransitionResource
      //CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      //  backBuffer.Get(),
      //  D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
      //);
      //// CommandContext::FlushResourceBarriers (also done in CommandContext::TransitionResource)
      //g_CommandList->ResourceBarrier(1, &barrier);

      //myContext.TransitionResource(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
      myContext.TransitionResource(backBuffer1, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

      FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
      // the rtv is in the g_SceneColorBuffer
      CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
        g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        g_CurrentBackBufferIndex, g_RTVDescriptorSize
      );

      //g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
      //myContext.ClearColor(rtv, clearColor);
      myContext.ClearColor(backBuffer1, clearColor);
    }

    // Present
    {
      //CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
      //  backBuffer.Get(),
      //  D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
      //);
      //g_CommandList->ResourceBarrier(1, &barrier);
      //myContext.TransitionResource(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, true);
      myContext.TransitionResource(backBuffer1, D3D12_RESOURCE_STATE_PRESENT, true);

      g_FrameFenceValues[g_CurrentBackBufferIndex] = myContext.Finish();

      //ThrowIfFailed(g_CommandList->Close());

      //ID3D12CommandList* const commandLists[] = {
      //    g_CommandList.Get()
      //};
      //_CommandManager->GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

      //g_FrameFenceValues[g_CurrentBackBufferIndex] = _CommandManager->GetGraphicsQueue().Signal();

      UINT syncInterval = g_VSync ? 1 : 0;
      UINT presentFlags = _TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
      ThrowIfFailed(g_SwapChain->Present(syncInterval, presentFlags));

      g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

      _CommandManager->GetGraphicsQueue().WaitForFence(g_FrameFenceValues[g_CurrentBackBufferIndex]); // TODO where does the Mini-Engine wait?
    }
  }

  void DX12Core::Resize(uint32_t width, uint32_t height) {
    if (_ClientWidth != width || _ClientHeight != height) {
      _ClientWidth = std::max(1u, width);
      _ClientHeight = std::max(1u, height);

      _CommandManager->IdleGpu();

      for (int i = 0; i < c_NumFrames; ++i) {
        g_BackBuffers1[i].Destroy();
        g_FrameFenceValues[i] = g_FrameFenceValues[g_CurrentBackBufferIndex];
      }

      DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
      ThrowIfFailed(g_SwapChain->GetDesc(&swapChainDesc));
      ThrowIfFailed(g_SwapChain->ResizeBuffers(
        c_NumFrames,
        _ClientWidth, _ClientHeight,
        swapChainDesc.BufferDesc.Format,
        swapChainDesc.Flags
      ));

      g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

      UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap);
    }
  }

  void DX12Core::ShutDown() {
    
    _CommandManager->IdleGpu();

    _CommandManager->Shutdown();

    _GlobalDescriptorAllocator->Shutdown();

    _IsInitialized = false;
  }

  void DX12Core::SetFullscreen(bool fullscreen) {
    if (_Fullscreen != fullscreen) {
      _TogglingFullscreen = true;
      _Fullscreen = fullscreen;

      if (_Fullscreen) {
        ::GetWindowRect(g_hWnd, &_WindowRect);

        UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        ::SetWindowLongW(g_hWnd, GWL_STYLE, windowStyle);

        HMONITOR hMonitor = ::MonitorFromWindow(g_hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(MONITORINFOEX);
        ::GetMonitorInfo(hMonitor, &monitorInfo);

        ::SetWindowPos(
          g_hWnd,
          HWND_TOP,
          monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
          monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
          monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
          SWP_FRAMECHANGED | SWP_NOACTIVATE
        );

        ::ShowWindow(g_hWnd, SW_MAXIMIZE);
      }
      else {

        ::SetWindowLong(g_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

        ::SetWindowPos(
          g_hWnd,
          HWND_NOTOPMOST,
          _WindowRect.left, _WindowRect.top,
          _WindowRect.right - _WindowRect.left,
          _WindowRect.bottom - _WindowRect.top,
          SWP_FRAMECHANGED | SWP_NOACTIVATE
        );

        ::ShowWindow(g_hWnd, SW_NORMAL);
      }
    }
  }

  void DX12Core::SwitchFullscreen() {
    bool fs = !_Fullscreen;
    SetFullscreen(fs);
    _Fullscreen = fs;
  }

}