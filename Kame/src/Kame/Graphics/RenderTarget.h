/**
 *
 */
#pragma once
 /*
  *  Copyright(c) 2018 Jeremiah van Oosten
  *
  *  Permission is hereby granted, free of charge, to any person obtaining a copy
  *  of this software and associated documentation files(the "Software"), to deal
  *  in the Software without restriction, including without limitation the rights
  *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
  *  copies of the Software, and to permit persons to whom the Software is
  *  furnished to do so, subject to the following conditions :
  *
  *  The above copyright notice and this permission notice shall be included in
  *  all copies or substantial portions of the Software.
  *
  *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
  *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  *  IN THE SOFTWARE.
  */

  /**
   *  @file RenderTarget.h
   *  @date October 24, 2018
   *  @author Jeremiah van Oosten
   *
   *  @brief A render target is used to store a set of textures that are the
   *  target for rendering.
   *  Maximum number of color textures that can be bound to the render target is 8
   *  (0 - 7) and one depth-stencil buffer.
   */

#include <DirectXMath.h> // For XMFLOAT2
#include <cstdint>
#include <vector>

//#include "TextureDx12.h"
#include <Kame/Graphics/RenderApi/Texture.h>

namespace Kame {

  // Don't use scoped enums to avoid the explicit cast required to use these as 
 // array indices.
  enum AttachmentPoint {
    Color0,
    Color1,
    Color2,
    Color3,
    Color4,
    Color5,
    Color6,
    Color7,
    DepthStencil,
    NumAttachmentPoints
  };

  class KAME_API RenderTarget {
  public:
    // Create an empty render target.
    RenderTarget();
    virtual ~RenderTarget();

    RenderTarget(const RenderTarget& copy) = default;
    RenderTarget(RenderTarget&& copy) = default;

    RenderTarget& operator=(const RenderTarget& other) = default;
    RenderTarget& operator=(RenderTarget&& other) = default;

    // Attach a texture to the render target.
    // The texture will be copied into the texture array.
    virtual void AttachTexture(AttachmentPoint attachmentPoint, Texture* texture) = 0;
    //const Texture& GetTexture(AttachmentPoint attachmentPoint) const;
    virtual const Texture* GetTexture(AttachmentPoint attachmentPoint) const = 0;

    // Resize all of the textures associated with the render target.
    virtual void Resize(DirectX::XMUINT2 size) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual DirectX::XMUINT2 GetSize() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    // Get a viewport for this render target.
    // The scale and bias parameters can be used to specify a split-screen
    // viewport (the bias parameter is normalized in the range [0...1]).
    // By default, a fullscreen viewport is returned.
    virtual D3D12_VIEWPORT GetViewport(
      DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, 
      DirectX::XMFLOAT2 bias = { 0.0f, 0.0f }, 
      float minDepth = 0.0f, float maxDepth = 1.0f
  ) const = 0;

    // Get the render target formats of the textures currently 
    // attached to this render target object.
    // This is needed to configure the Pipeline state object.
    virtual D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const = 0;

    // Get the format of the attached depth/stencil buffer.
    virtual DXGI_FORMAT GetDepthStencilFormat() const = 0;

  protected:

    DirectX::XMUINT2 m_Size;
  };

  template<class TextureType>
  class RenderTargetOf : public RenderTarget {
  public:
    RenderTargetOf();
    virtual ~RenderTargetOf();

    virtual void AttachTexture(AttachmentPoint attachmentPoint, Texture* texture) override;

    virtual const Texture* GetTexture(AttachmentPoint attachmentPoint) const override;

    virtual void Resize(DirectX::XMUINT2 size) override ;
    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual DirectX::XMUINT2 GetSize() const override;
    virtual uint32_t GetWidth() const override;
    virtual uint32_t GetHeight() const override;
   
    virtual D3D12_VIEWPORT GetViewport(
      DirectX::XMFLOAT2 scale = { 1.0f, 1.0f },
      DirectX::XMFLOAT2 bias = { 0.0f, 0.0f },
      float minDepth = 0.0f, float maxDepth = 1.0f
    ) const override;
    
    virtual D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const override;

    virtual DXGI_FORMAT GetDepthStencilFormat() const override;

  protected:
    std::vector<TextureType*> m_Textures;
  };

  template<class TextureType>
  RenderTargetOf<TextureType>::RenderTargetOf()
    : m_Textures(AttachmentPoint::NumAttachmentPoints) {}

  template<class TextureType>
  inline RenderTargetOf<TextureType>::~RenderTargetOf() {
  }

  template<class TextureType>
  inline void Kame::RenderTargetOf<TextureType>::AttachTexture(AttachmentPoint attachmentPoint, Texture* texture) {
    TextureType* x = static_cast<TextureType*>(texture);
    m_Textures[attachmentPoint] = x;

    if (texture->IsValid1()) {
      m_Size.x = static_cast<uint32_t>(texture->GetWidth());
      m_Size.y = static_cast<uint32_t>(texture->GetHeight());
    }
  }

  template<class TextureType>
  const Texture* RenderTargetOf<TextureType>::GetTexture(AttachmentPoint attachmentPoint) const {
    return m_Textures[attachmentPoint];
  }

  template<class TextureType>
  void RenderTargetOf<TextureType>::Resize(DirectX::XMUINT2 size) {
    m_Size = size;
    for (auto& texture : m_Textures) {
      if (!texture)
        continue;
      texture->Resize(m_Size.x, m_Size.y);
    }

  }

  template<class TextureType>
  void RenderTargetOf<TextureType>::Resize(uint32_t width, uint32_t height) {
    Resize(DirectX::XMUINT2(width, height));
  }

  template<class TextureType>
  DirectX::XMUINT2 RenderTargetOf<TextureType>::GetSize() const {
    return m_Size;
  }

  template<class TextureType>
  uint32_t RenderTargetOf<TextureType>::GetWidth() const {
    return m_Size.x;
  }

  template<class TextureType>
  uint32_t RenderTargetOf<TextureType>::GetHeight() const {
    return m_Size.y;
  }

  template<class TextureType>
  D3D12_VIEWPORT RenderTargetOf<TextureType>::GetViewport(DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 bias, float minDepth, float maxDepth) const {
    UINT64 width = 0;
    UINT64 height = 0;

    for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i) {
      const auto texture = m_Textures[i];
      if (!texture)
        continue;
      if (texture->IsValid1()) {
        width = std::max(width, texture->GetWidth());
        height = std::max(height, texture->GetHeight());
      }
    }

    D3D12_VIEWPORT viewport = {
        (width * bias.x),       // TopLeftX
        (height * bias.y),      // TopLeftY
        (width * scale.x),      // Width
        (height * scale.y),     // Height
        minDepth,               // MinDepth
        maxDepth                // MaxDepth
    };

    return viewport;
  }

  template<class TextureType>
  D3D12_RT_FORMAT_ARRAY RenderTargetOf<TextureType>::GetRenderTargetFormats() const {
    D3D12_RT_FORMAT_ARRAY rtvFormats = {};


    for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color0; ++i) {
      const auto texture = m_Textures[i];
      if (!texture)
        continue;
      if (texture->IsValid1()) {
        rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture->GetFormat();
      }
    }

    return rtvFormats;
  }

  template<class TextureType>
  DXGI_FORMAT RenderTargetOf<TextureType>::GetDepthStencilFormat() const {
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
    const auto depthStencilTexture = m_Textures[AttachmentPoint::DepthStencil];
    if (!depthStencilTexture)
      return dsvFormat;
    if (depthStencilTexture->IsValid1()) {
      dsvFormat = depthStencilTexture->GetFormat();
    }

    return dsvFormat;
  }

}