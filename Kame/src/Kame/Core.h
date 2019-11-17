#pragma once

#ifdef KAME_PLATFORM_WINDOWS
#ifdef KAME_BUILD_DLL
#define KAME_API __declspec(dllexport)
#else
#define KAME_API __declspec(dllimport)
#endif
#else
#error Kame only supports Windows!
#endif

#ifdef KAME_DEBUG
#define KM_ASSERT(x, ...) { if(!(x)) {KM_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define KM_CORE_ASSERT(x, ...) { if(!(x)) {KM_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define KM_ASSERT(x, ...)
#define KM_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define KM_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#if defined KAME_PLATFORM_WINDOWS
#include <Windows.h>
#include <vcruntime_exception.h>
inline void ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr)) {
    throw std::exception();
  }
}
#endif
