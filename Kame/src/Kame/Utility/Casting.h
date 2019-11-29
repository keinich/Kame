#pragma once

namespace Kame {

  template <typename DstType, typename SrcType>
  DstType* ValidatedCast(SrcType* Ptr) {
#ifdef _DEBUG
    if (Ptr != nullptr) {
      dynamic_cast<DstType>(Ptr); //TODO validate...
    }
#endif
    return static_cast<DstType*>(Ptr);
  }
}