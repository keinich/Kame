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