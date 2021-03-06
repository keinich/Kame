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
  *  @file Application.h
  *  @date October 22, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief The application class is used to create windows for our application.
  */

  //System
#include <string>
#include <memory>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

//Kame
#include "DescriptorAllocation.h"
#include "Kame/Graphics/GraphicsCore.h"
#include <Kame/Core/References.h>

namespace Kame {

  class CommandQueue;
  class DescriptorAllocator;
  class Game;
  class Display;

  class DX12Core : public GraphicsCore {

    friend class Application;

  public:

    /**
    * Create the application singleton with the application instance handle.
    */
    static void Create();

    /**
    * Destroy the application instance and all windows created by this application instance.
    */
    static void Destroy();

    /**
    * Get the application singleton.
    */
    static DX12Core& Get();

    // Initialize the application instance.
    void Initialize();

    /**
     * Check to see if VSync-off is supported.
     */
    bool IsTearingSupported() const;

    /**
     * Check if the requested multisample quality is supported for the given format.
     */
    DXGI_SAMPLE_DESC GetMultisampleQualityLevels(DXGI_FORMAT format, UINT numSamples, D3D12_MULTISAMPLE_QUALITY_LEVEL_FLAGS flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE) const;

    /**
    * Create a new DirectX11 render window instance.
    * @param windowName The name of the window. This name will appear in the title bar of the window. This name should be unique.
    * @param clientWidth The width (in pixels) of the window's client area.
    * @param clientHeight The height (in pixels) of the window's client area.
    * @param vSync Should the rendering be synchronized with the vertical refresh rate of the screen.
    * @param windowed If true, the window will be created in windowed mode. If false, the window will be created full-screen.
    * @returns The created window instance. If an error occurred while creating the window an invalid
    * window instance is returned. If a window with the given name already exists, that window will be
    * returned.
    */
    //std::shared_ptr<Window> CreateRenderWindow(const std::wstring& windowName, int clientWidth, int clientHeight, bool vSync = true);

    /**
    * Destroy a window given the window name.
    */
    //void DestroyWindow(const std::wstring& windowName);
    /**
    * Destroy a window given the window reference.
    */
    //void DestroyWindow(std::shared_ptr<Window> window);

    /**
    * Find a window by the window name.
    */
    //std::shared_ptr<Window> GetWindowByName(const std::wstring& windowName);

    /**
    * Run the application loop and message pump.
    * @return The error code if an error occurred.
    */
    //int Run(std::shared_ptr<Game> pGame);

    /**
    * Request to quit the application and close all windows.
    * @param exitCode The error code to return to the invoking process.
    */
    //void Quit(int exitCode = 0);

    /**
     * Get the Direct3D 12 device
     */
    Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice() const;
    /**
     * Get a command queue. Valid types are:
     * - D3D12_COMMAND_LIST_TYPE_DIRECT : Can be used for draw, dispatch, or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COMPUTE: Can be used for dispatch or copy commands.
     * - D3D12_COMMAND_LIST_TYPE_COPY   : Can be used for copy commands.
     */
    virtual Reference<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const override;

    /**
     * Flush all command queues.
     */
    void Flush();

    /**
     * Allocate a number of CPU visible descriptors.
     */
    DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors = 1);

    /**
     * Release stale descriptors. This should only be called with a completed frame counter.
     */
    void ReleaseStaleDescriptors(uint64_t finishedFrame);

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE type);
    UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type) const;

    static uint64_t GetFrameCount() {
      return ms_FrameCount;
    }

    D3D_ROOT_SIGNATURE_VERSION GetHighestRootSignatureVersion() { return  _RootSignatureFeatureData.HighestVersion; }

  protected:

    // Create an application instance.
    DX12Core();
    // Destroy the application instance and all windows associated with this application.
    virtual ~DX12Core();


    Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool bUseWarp);
    Microsoft::WRL::ComPtr<ID3D12Device2> CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter);
    bool CheckTearingSupport();

  private:
    friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    DX12Core(const DX12Core& copy) = delete;
    DX12Core& operator=(const DX12Core& other) = delete;

    HINSTANCE m_hInstance;

    Microsoft::WRL::ComPtr<ID3D12Device2> m_d3d12Device;

    Reference<CommandQueue> m_DirectCommandQueue;
    Reference<CommandQueue> m_ComputeCommandQueue;
    Reference<CommandQueue> m_CopyCommandQueue;

    NotCopyableReference<DescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

    bool m_TearingSupported;

    static uint64_t ms_FrameCount;

    D3D12_FEATURE_DATA_ROOT_SIGNATURE _RootSignatureFeatureData;
  };

}
