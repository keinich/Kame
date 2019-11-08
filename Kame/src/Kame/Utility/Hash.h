#pragma once
#include <cstdint>

#include "Kame/Math/MathCommon.h"

#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif

namespace Kame {
  namespace Utility {

    inline size_t HashRange(const uint32_t* const begin, const uint32_t* const end, size_t hash) {

#if ENABLE_SSE_CRC32

      const uint64_t* iter64 = (const uint64_t*)Math::AlignUp(begin, 8);
      const uint64_t* const end64 = (const uint64_t* const)Math::AlignDown(end, 8);

      // If not 64-bit aligned, start with a single u32
      if ((uint32_t*)iter64 > begin)
        hash = _mm_crc32_u32((uint32_t)hash, *begin);

      // Iterate over consecutive u64 values
      while (iter64 < end64)
        hash = _mm_crc32_u64((uint64_t)hash, *iter64++);

      // If there is a 32-bit remainder, accumulate that
      if ((uint32_t*)iter64 < end)
        hash = _mm_crc32_u32((uint32_t)hash, *(uint32_t*)iter64);
#else
      // An inexpensive hash for CPUs lacking SSE4.2
      for (const uint32_t* iter = begin; Iter < end; ++iter)
        Hash = 16777619U * hash ^ *iter;

#endif

      return hash;

    }

    template <typename T> inline size_t HashState(const T* stateDesc, size_t count = 1, size_t hash = 2166136261U) {
      static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");
      return HashRange((uint32_t*)stateDesc, (uint32_t*)(stateDesc + count), hash);
    }

  }
}