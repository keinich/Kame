#include "kmpch.h"
#include "DxTutorial.h"
#include "Kame/Platform/Win32/Win32Window.h"
#include "Kame/Application.h"

namespace Kame {

  DxTutorial* DxTutorial::_Instance = new DxTutorial();

  void DxTutorial::Init() {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    EnableDebugLayer();
    _TearingSupported = CheckTearingSupport();

    //Window* w = Window::Create();
    //Window* w = &Application::Get().GetWindow();

    WindowProperties wp;
    Win32Window w(wp);

    Window* window = &(Application::Get().GetWindow());
    Win32Window* win32Window = (Win32Window*)window;   
    
    g_hWnd = (HWND)Application::Get().GetWindow().GetNativeWindow();   

    //g_hWnd = (HWND)(w->GetNativeWindow());
    ::GetWindowRect(g_hWnd, &_WindowRect);

    ComPtr<IDXGIAdapter4> dxgiAdapter4 = GetAdapter(_UseWarp);

    g_Device = CreateDevice(dxgiAdapter4);

    g_CommandQueue = CreateCommandQueue(g_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

    g_SwapChain = CreateSwapChain(g_hWnd, g_CommandQueue, _ClientWidth, _ClientHeight, c_NumFrames);
    
    g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

    g_RTVDescriptorHeap = CreateDescriptorHeap(g_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, c_NumFrames);
    g_RTVDescriptorSize = g_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    UpdateRenderTargetViews(g_Device, g_SwapChain, g_RTVDescriptorHeap);

    for (int i = 0; i < c_NumFrames; ++i) {
      g_CommandAllocator[i] = CreateCommandAllocator(g_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }
    g_CommandList = CreateCommandList(g_Device, g_CommandAllocator[g_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

    _Fence = CreateFence(g_Device);
    g_FenceEvent = CreateEventHandle();

    _IsInitialized = true;

    /*::ShowWindow(g_hWnd, SW_SHOW);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
      if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    }*/   
        
  }

  void DxTutorial::EnableDebugLayer() {

#if defined KAME_DEBUG

    ComPtr<ID3D12Debug> debugInterface;
    HRESULT r = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    ThrowIfFailed(r);
    debugInterface->EnableDebugLayer();

#endif

  }

  ComPtr<IDXGIAdapter4> DxTutorial::GetAdapter(bool useWarp) {
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

  ComPtr<ID3D12Device2> DxTutorial::CreateDevice(ComPtr<IDXGIAdapter4> adapter) {

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

  ComPtr<ID3D12CommandQueue> DxTutorial::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

    return d3d12CommandQueue;

  }

  bool DxTutorial::CheckTearingSupport() {
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

  ComPtr<IDXGISwapChain4> DxTutorial::CreateSwapChain(
    HWND hWnd,
    ComPtr<ID3D12CommandQueue> commandQueue,
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
      commandQueue.Get(),
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

  ComPtr<ID3D12DescriptorHeap> DxTutorial::CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;

    ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

    return descriptorHeap;
  }

  void DxTutorial::UpdateRenderTargetViews(
    ComPtr<ID3D12Device2> device,
    ComPtr<IDXGISwapChain4> swapChain,
    ComPtr<ID3D12DescriptorHeap> descriptorHeap
  ) {
    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < c_NumFrames; ++i) {
      ComPtr<ID3D12Resource> backBuffer;
      ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

      device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

      g_BackBuffers[i] = backBuffer;

      rtvHandle.Offset(rtvDescriptorSize);

    }
  }

  ComPtr<ID3D12CommandAllocator> Kame::DxTutorial::CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));

    return commandAllocator;
  }

  ComPtr<ID3D12GraphicsCommandList> DxTutorial::CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type) {
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

    ThrowIfFailed(commandList->Close());

    return commandList;
  }

  ComPtr<ID3D12Fence> DxTutorial::CreateFence(ComPtr<ID3D12Device2> device) {
    ComPtr<ID3D12Fence> fence;

    ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    return fence;
  }

  HANDLE DxTutorial::CreateEventHandle() {
    HANDLE fenceEvent;

    fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(fenceEvent && "Failed to create fence event.");

    return fenceEvent;
  }

  uint64_t DxTutorial::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence, uint64_t& fenceValue) {
    uint64_t fenceValueForSignal = ++fenceValue;
    ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValue));

    return fenceValueForSignal;
  }

  void DxTutorial::WaitForFenceValue(
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

  void DxTutorial::Flush(
    ComPtr<ID3D12CommandQueue> commandQueue,
    ComPtr<ID3D12Fence> fence,
    uint64_t& fenceValue,
    HANDLE fenceEvent
  ) {
    uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
    WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
  }

  void DxTutorial::Update() {
    static uint64_t frameCounter = 0;
    static double elapsedSeconds = 0.0;
    static std::chrono::high_resolution_clock clock;
    static auto t0 = clock.now();

    frameCounter++;
    auto t1 = clock.now();
    auto deltaTime = t1 - t0;
    t0 = t1;

    elapsedSeconds += deltaTime.count() * 1e-9;
    if (elapsedSeconds > 1.0) {
      char buffer[500];
      auto fps = frameCounter / elapsedSeconds;
      sprintf_s(buffer, 500, "FPS: %f\n", fps);
      KM_INFO(buffer);

      frameCounter = 0;
      elapsedSeconds = 0.0;
    }

  }

  void Kame::DxTutorial::Render() {
    auto commandAllocator = g_CommandAllocator[g_CurrentBackBufferIndex];
    auto backBuffer = g_BackBuffers[g_CurrentBackBufferIndex];

    commandAllocator->Reset();
    g_CommandList->Reset(commandAllocator.Get(), nullptr);

    // Clear the render target.
    {
      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

      g_CommandList->ResourceBarrier(1, &barrier);

      FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
      CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        g_CurrentBackBufferIndex, g_RTVDescriptorSize);

      g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    // Present
    {
      CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
      g_CommandList->ResourceBarrier(1, &barrier);

      ThrowIfFailed(g_CommandList->Close());

      ID3D12CommandList* const commandLists[] = {
          g_CommandList.Get()
      };
      g_CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

      g_FrameFenceValues[g_CurrentBackBufferIndex] = Signal(g_CommandQueue, _Fence, g_FenceValue);

      UINT syncInterval = g_VSync ? 1 : 0;
      UINT presentFlags = _TearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
      ThrowIfFailed(g_SwapChain->Present(syncInterval, presentFlags));

      g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

      WaitForFenceValue(_Fence, g_FrameFenceValues[g_CurrentBackBufferIndex], g_FenceEvent);
    }
  }

  void DxTutorial::Resize(uint32_t width, uint32_t height) {
    if (_ClientWidth != width || _ClientHeight != height) {
      _ClientWidth = std::max(1u, width);
      _ClientHeight = std::max(1u, height);

      Flush(g_CommandQueue, _Fence, g_FenceValue, g_FenceEvent);

      for (int i = 0; i < c_NumFrames; ++i) {
        g_BackBuffers[i].Reset();
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

  void DxTutorial::ShutDown() {
    Flush(g_CommandQueue, _Fence, g_FenceValue, g_FenceEvent);

    ::CloseHandle(g_FenceEvent);
  }

  void DxTutorial::SetFullscreen(bool fullscreen) {
    if (_Fullscreen != fullscreen) {
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

}