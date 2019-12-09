#pragma once

#include <d3d12.h> //TODO abstract away D3D12_ROOT_SIGNATURE_DESC1, D3D_ROOT_SIGNATURE_VERSION

namespace Kame {

  class RenderProgramSignature {

  public:

    virtual ~RenderProgramSignature() {};

    virtual void SetDescription(
      const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc
    ) = 0;

    virtual size_t GetIdentifier() = 0;

  };

}