#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <unordered_set>

#include <chrono>

#include "Kame/Log.h"

//#include "Kame/VectorMath.h"

#ifdef KAME_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif

#include <Windows.h>


#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#if defined(CreateWindow)
#undef CreateWindow
#endif

#include <wrl.h>

#ifdef KAME_PLATFORM_DIRECTX12

#include <initguid.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Kame/Platform/DirectX12/Graphics/dxd12.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Kame/Platform/DirectX12/Math/DX12Math.h"

#define INTERNAL_SCALAR_REPRESENTATION DirectX::XMVECTOR

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include <DirectXMath.h>

using namespace Microsoft::WRL;

#define MY_IID_PPV_ARGS IID_PPV_ARGS
#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#endif


#endif

