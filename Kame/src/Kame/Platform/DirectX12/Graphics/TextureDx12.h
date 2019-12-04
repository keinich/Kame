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
  *  @file Texture.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A wrapper for a DX12 Texture object.
  */


#include "GpuResourceDx12.h"
#include "DescriptorAllocation.h"
#include <Kame/Graphics/RenderApi/TextureUsage.h>

#include "d3dx12.h"

#include <mutex>
#include <unordered_map>

#include "Kame/Graphics/RenderApi/Texture.h"

namespace Kame {

  class KAME_API TextureDx12 : public GpuResourceDx12, public Texture {
  public:
    explicit TextureDx12(TextureUsage textureUsage = TextureUsage::Albedo,
      const std::wstring& name = L"");
    explicit TextureDx12(const D3D12_RESOURCE_DESC& resourceDesc,
      const D3D12_CLEAR_VALUE* clearValue = nullptr,
      TextureUsage textureUsage = TextureUsage::Albedo,
      const std::wstring& name = L"");
    explicit TextureDx12(Microsoft::WRL::ComPtr<ID3D12Resource> resource,
      TextureUsage textureUsage = TextureUsage::Albedo,
      const std::wstring& name = L"");

    TextureDx12(const TextureDx12& copy);
    TextureDx12(TextureDx12&& copy);

    TextureDx12& operator=(const TextureDx12& other);
    TextureDx12& operator=(TextureDx12&& other);

    virtual ~TextureDx12();

    virtual void LoadFromFile(const std::wstring& fileName, TextureUsage textureUsage) override;

    virtual Reference<Texture> ToCubeMap(UINT width, std::wstring name) override;

    TextureUsage GetTextureUsage() const {
      return m_TextureUsage;
    }

    void SetTextureUsage(TextureUsage textureUsage) {
      m_TextureUsage = textureUsage;
    }

    /**
     * Resize the texture.
     */
    virtual void Resize(uint32_t width, uint32_t height, uint32_t depthOrArraySize = 1) override;

    /**
     * Create SRV and UAVs for the resource.
     */
    virtual void CreateViews();

    //inline virtual void* GetNativeHandle() const { return m_d3d12Resource.Get(); }
    inline virtual bool IsValid1() const override { return IsValid(); }

    inline virtual UINT64 GetWidth() const override { return m_d3d12Resource->GetDesc().Width; }
    inline virtual UINT64 GetHeight() const override { return m_d3d12Resource->GetDesc().Height; }

    inline virtual DXGI_FORMAT GetFormat() const override { return m_d3d12Resource->GetDesc().Format; }

    /**
    * Get the SRV for a resource.
    */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc = nullptr) const override;

    /**
    * Get the UAV for a (sub)resource.
    */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc = nullptr) const override;

    /**
     * Get the RTV for the texture.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

    /**
     * Get the DSV for the texture.
     */
    virtual D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    bool CheckSRVSupport() {
      return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
    }

    bool CheckRTVSupport() {
      return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
    }

    bool CheckUAVSupport() {
      return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
        CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
        CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
    }

    bool CheckDSVSupport() {
      return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
    }

    static bool IsUAVCompatibleFormat(DXGI_FORMAT format);
    static bool IsSRGBFormat(DXGI_FORMAT format);
    static bool IsBGRFormat(DXGI_FORMAT format);
    static bool IsDepthFormat(DXGI_FORMAT format);

    // Return a typeless format from the given format.
    static DXGI_FORMAT GetTypelessFormat(DXGI_FORMAT format);
    static DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT format);

  protected:

  private:
    DescriptorAllocation CreateShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc) const;
    DescriptorAllocation CreateUnorderedAccessView(const D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc) const;

    mutable std::unordered_map<size_t, DescriptorAllocation> m_ShaderResourceViews;
    mutable std::unordered_map<size_t, DescriptorAllocation> m_UnorderedAccessViews;

    mutable std::mutex m_ShaderResourceViewsMutex;
    mutable std::mutex m_UnorderedAccessViewsMutex;

    DescriptorAllocation m_RenderTargetView;
    DescriptorAllocation m_DepthStencilView;

    TextureUsage m_TextureUsage;
  };

}