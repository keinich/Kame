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

  struct VertexPosColor {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Color;
  };

  static VertexPosColor _Vertices[8] = {
    { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
    { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
    { DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
    { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
    { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
    { DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
    { DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
    { DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
  };

  static WORD _Indices[36] = {
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
  };

  DX12Core* DX12Core::_Instance = new DX12Core();

  DescriptorAllocator g_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] = {
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
    D3D12_DESCRIPTOR_HEAP_TYPE_DSV
  };

  DX12Core::DX12Core() :
    _ScissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX)),
    _Viewport(CD3DX12_VIEWPORT(0.0f, 0.0f, _ClientWidth, _ClientHeight)),
    _FoV(45.0f),
    _ContentLoaded(false) {
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

    _Device = CreateDevice(dxgiAdapter4);

    _CommandManager->Create(_Device.Get());

    g_SwapChain = CreateSwapChain(g_hWnd, _CommandManager->GetCommandQueue(), _ClientWidth, _ClientHeight, c_NumFrames);

    g_CurrentBackBufferIndex = g_SwapChain->GetCurrentBackBufferIndex();

    //g_RTVDescriptorHeap = CreateDescriptorHeap(_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, c_NumFrames);
    //g_RTVDescriptorSize = _Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //UpdateRenderTargetViews(_Device, g_SwapChain, g_RTVDescriptorHeap);
    UpdateRenderTargetViews(_Device, g_SwapChain);

    for (int i = 0; i < c_NumFrames; ++i) {
      g_CommandAllocator[i] = CreateCommandAllocator(_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    }
    g_CommandList = CreateCommandList(_Device, g_CommandAllocator[g_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

    _IsInitialized = true;

    LoadContent();

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
    ComPtr<IDXGISwapChain4> swapChain
  ) {
    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    //CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < c_NumFrames; ++i) {
      ComPtr<ID3D12Resource> backBuffer;
      ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

      g_BackBuffers1[i].CreateFromSwapChain(L"", backBuffer.Detach());

      //device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

      //g_BackBuffers[i] = backBuffer;

      //rtvHandle.Offset(rtvDescriptorSize);

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

  void DX12Core::UpdateBufferResource(
    ID3D12GraphicsCommandList* commandList,
    ID3D12Resource** destinationResource,
    ID3D12Resource** intermediateResource,
    size_t numElements, size_t elementSize,
    const void* bufferData,
    D3D12_RESOURCE_FLAGS flags
  ) {

    size_t bufferSize = numElements * elementSize;

    ThrowIfFailed(
      _Device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(destinationResource)
      )
    );

    if (bufferData) {
      ThrowIfFailed(
        _Device->CreateCommittedResource(
          &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
          D3D12_HEAP_FLAG_NONE,
          &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
          D3D12_RESOURCE_STATE_GENERIC_READ,
          nullptr,
          IID_PPV_ARGS(intermediateResource)
        )
      );

      D3D12_SUBRESOURCE_DATA subresourceData = {};
      subresourceData.pData = bufferData;
      subresourceData.RowPitch = bufferSize;
      subresourceData.SlicePitch = subresourceData.RowPitch;

      UpdateSubresources(
        commandList,
        *destinationResource, *intermediateResource,
        0, 0, 1, &subresourceData
      );

    }

  }

  bool DX12Core::LoadContent() {
    CommandContext& initContext = CommandContext::Begin(L"Test");
    ID3D12GraphicsCommandList* commandList = initContext.GetCommandList();

    ComPtr<ID3D12Resource> intermediateVertexBuffer;
    UpdateBufferResource(commandList, &_VertexBuffer, &intermediateVertexBuffer, _countof(_Vertices), sizeof(VertexPosColor), _Vertices);

    _VertexBufferView.BufferLocation = _VertexBuffer->GetGPUVirtualAddress();
    _VertexBufferView.SizeInBytes = sizeof(_Vertices);
    _VertexBufferView.StrideInBytes = sizeof(VertexPosColor);

    ComPtr<ID3D12Resource> intermediateIndexBuffer;
    UpdateBufferResource(commandList, &_IndexBuffer, &intermediateIndexBuffer, _countof(_Indices), sizeof(WORD), _Indices);

    _IndexBufferView.BufferLocation = _IndexBuffer->GetGPUVirtualAddress();
    _IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    _IndexBufferView.SizeInBytes = sizeof(_Indices);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&_DsvHeap)));

    ComPtr<ID3DBlob> vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Kame\\VertexShader.cso", &vertexShaderBlob));
    ComPtr<ID3DBlob> pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"D:\\Raftek\\Kame\\bin\\Debug-windows-x86_64\\Kame\\PixelShader.cso", &pixelShaderBlob));

    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))) {
      featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
      D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_ROOT_PARAMETER1 rootParameters[1];
    rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription, featureData.HighestVersion, &rootSignatureBlob, &errorBlob));
    ThrowIfFailed(_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&_RootSignature)));

    struct PipelineStateStream {
      CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
      CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
      CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
      CD3DX12_PIPELINE_STATE_STREAM_VS VS;
      CD3DX12_PIPELINE_STATE_STREAM_PS PS;
      CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
      CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    } pipelineStateStream;

    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 1;
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineStateStream.pRootSignature = _RootSignature.Get();
    pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
    pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
    pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
    pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineStateStream.RTVFormats = rtvFormats;

    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };
    ThrowIfFailed(_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&_PipelineState)));

    initContext.Finish(true);

    _ContentLoaded = true;

    ResizeDepthBuffer(_ClientWidth, _ClientHeight);

    return true;
  }

  void DX12Core::ResizeDepthBuffer(int width, int height) {

    if (_ContentLoaded) {

      _CommandManager->IdleGpu();

      width = std::max(1, width);
      height = std::max(1, height);

      auto device = _Device;

      // Resize screen dependent resources.
      // Create a depth buffer.
      D3D12_CLEAR_VALUE optimizedClearValue = {};
      optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
      optimizedClearValue.DepthStencil = { 1.0f, 0 };

      ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height,
          1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,
        IID_PPV_ARGS(&_DepthBuffer)
      ));

      // Update the depth-stencil view.
      D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
      dsv.Format = DXGI_FORMAT_D32_FLOAT;
      dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
      dsv.Texture2D.MipSlice = 0;
      dsv.Flags = D3D12_DSV_FLAG_NONE;

      device->CreateDepthStencilView(_DepthBuffer.Get(), &dsv,
        _DsvHeap->GetCPUDescriptorHandleForHeapStart());

    }

  }

  void DX12Core::GameUpdate() {

    static float angle = 0.0f;
    angle += 0.1;

    const DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
    _ModelMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));

    // Update the view matrix.
    const DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
    const DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
    const DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
    _ViewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

    // Update the projection matrix.
    float aspectRatio = _ClientWidth / static_cast<float>(_ClientHeight);
    _ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(_FoV), aspectRatio, 0.1f, 100.0f);

  }

  void DX12Core::Render() {

    GameUpdate();

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

    auto dsv = _DsvHeap->GetCPUDescriptorHandleForHeapStart();

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
      /*CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
        g_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
        g_CurrentBackBufferIndex
      );*/

      //g_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
      //myContext.ClearColor(rtv, clearColor);
      myContext.ClearColor(backBuffer1, clearColor);

      myContext.GetCommandList()->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }

    auto commandList = myContext.GetCommandList();

    commandList->SetPipelineState(_PipelineState.Get());
    commandList->SetGraphicsRootSignature(_RootSignature.Get());

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &_VertexBufferView);
    commandList->IASetIndexBuffer(&_IndexBufferView);

    commandList->RSSetViewports(1, &_Viewport);
    commandList->RSSetScissorRects(1, &_ScissorRect);

    commandList->OMSetRenderTargets(1, &backBuffer1.GetRtv(), FALSE, &dsv);

    // Update the MVP matrix
    DirectX::XMMATRIX mvpMatrix = DirectX::XMMatrixMultiply(_ModelMatrix, _ViewMatrix);
    mvpMatrix = XMMatrixMultiply(mvpMatrix, _ProjectionMatrix);
    commandList->SetGraphicsRoot32BitConstants(0, sizeof(DirectX::XMMATRIX) / 4, &mvpMatrix, 0);

    commandList->DrawIndexedInstanced(_countof(_Indices), 1, 0, 0, 0);



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

      //UpdateRenderTargetViews(_Device, g_SwapChain, g_RTVDescriptorHeap);
      UpdateRenderTargetViews(_Device, g_SwapChain);

      // Game.OnResize
      _Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(_ClientWidth), static_cast<float>(_ClientHeight));

      ResizeDepthBuffer(_ClientWidth, _ClientHeight);
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