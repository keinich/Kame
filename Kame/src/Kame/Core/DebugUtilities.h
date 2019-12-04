#pragma once

#include <memory>

#ifdef KAME_DEBUG
#define KM_ASSERT(x, ...) { if(!(x)) {KM_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define KM_CORE_ASSERT(x, ...) { if(!(x)) {KM_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define KM_ASSERT(x, ...)
#define KM_CORE_ASSERT(x, ...)
#endif

#if defined KAME_PLATFORM_WINDOWS
#include <Windows.h>
#include <vcruntime_exception.h>
inline void ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr)) {
    throw std::exception();
  }
}
#endif