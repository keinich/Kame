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
  *  @file Window.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A window for our application.
  */

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_5.h>

#include "Events.h"
#include "GUI.h"
#include "HighResolutionClock.h"
#include "RenderTarget.h"
#include "Texture.h"

#include <memory>

namespace Kame {

  class Game;
  class Texture;

  class KAME_API Display : public std::enable_shared_from_this<Display> {

    friend class Window;
    friend class Win32Window;

  public:
    // Number of swapchain back buffers.
    static const UINT BufferCount = 3;

    /**
     * Initialize the window.
     */
    void Initialize();

    int GetClientWidth() const;
    int GetClientHeight() const;

    /**
    * Should this window be rendered with vertical refresh synchronization.
    */
    bool IsVSync() const;
    void SetVSync(bool vSync);
    void ToggleVSync();

    /**
     * Get the render target of the window. This method should be called every
     * frame since the color attachment point changes depending on the window's
     * current back buffer.
     */
    const RenderTarget& GetRenderTarget() const;

    /**
     * Present the swapchain's back buffer to the screen.
     * Returns the current back buffer index after the present.
     *
     * @param texture The texture to copy to the swap chain's backbuffer before
     * presenting. By default, this is an empty texture. In this case, no copy
     * will be performed. Use the Window::GetRenderTarget method to get a render
     * target for the window's color buffer.
     */
    UINT Present(const Texture& texture = Texture());

  protected:

    // Only the application can create a window.
    friend class DX12Core;
    // The DirectXTemplate class needs to register itself with a window.
    friend class Game;

    Display() = delete;
    Display(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync);
    virtual ~Display();

    void Initialize(HWND hWnd);

    // The window was resized.
    virtual void OnResize(ResizeEventArgs& e);

    // Create the swapchian.
    Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(HWND hWnd);

    // Update the render target views for the swapchain back buffers.
    void UpdateRenderTargetViews();

  private:
    // Windows should not be copied.
    Display(const Display& copy) = delete;
    Display& operator=(const Display& other) = delete;

    int m_ClientWidth;
    int m_ClientHeight;
    bool m_VSync;

    UINT64 m_FenceValues[BufferCount];
    uint64_t m_FrameValues[BufferCount];

    Microsoft::WRL::ComPtr<IDXGISwapChain4> m_dxgiSwapChain;
    Texture m_BackBufferTextures[BufferCount];
    // Marked mutable to allow modification in a const function.
    mutable RenderTarget m_RenderTarget;

    UINT m_CurrentBackBufferIndex;

    bool m_IsTearingSupported;



    //GUI m_GUI;

  };

}