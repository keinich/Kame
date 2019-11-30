#pragma once

#include <memory>

#ifdef KAME_PLATFORM_WINDOWS
#if KAME_DYNAMIC_LINK
#ifdef KAME_BUILD_DLL
#define KAME_API __declspec(dllexport)
#else
#define KAME_API __declspec(dllimport)
#endif
#else
#define KAME_API 
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

namespace Kame {

  template<typename T>
  using NotCopyableReference = std::unique_ptr<T>;
  template<typename T, typename ... Args>
  constexpr NotCopyableReference<T> CreateNotCopyableReference(Args&& ... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
  }

  template<typename T>
  using Reference = std::shared_ptr<T>;
  template<typename T, typename ... Args>
  constexpr Reference<T> CreateReference(Args&& ... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
}