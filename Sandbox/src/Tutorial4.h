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
  *  @file Tutorial4.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Tutorial 4.
  */


#include "Kame/Graphics/Camera.h"
#include "Kame/Game/Game.h"
#include "Kame/Platform/DirectX12/Graphics/IndexBufferDx12.h"
#include "Kame/Graphics/Light.h"
#include "Kame/Platform/DirectX12/Graphics/Display.h"
#include "Kame/Graphics/Mesh.h"
#include "Kame/Graphics/RenderTarget.h"
#include "Kame/Platform/DirectX12/Graphics/RootSignatureDx12.h"
#include "Kame/Platform/DirectX12/Graphics/TextureDx12.h"
#include "Kame/Platform/DirectX12/Graphics/VertexBufferDx12.h"

#include "Kame/Math/Vector4.h"

#include <DirectXMath.h>

namespace Kame {

  class Tutorial4 : public Game {
  public:
    using super = Game;

    Tutorial4(const std::wstring& name, int width, int height, bool vSync = false);
    virtual ~Tutorial4();

    /**
     *  Load content required for the demo.
     */
    virtual bool LoadContent() override;

    /**
     *  Unload demo specific content that was loaded in LoadContent.
     */
    virtual void UnloadContent() override;
  protected:
    /**
     *  Update the game logic.
     */
    virtual void OnUpdate(UpdateEventArgs& e) override;

    /**
     *  Render stuff.
     */
    virtual void OnRender(RenderEventArgs& e) override;

    /**
     * Invoked by the registered window when a key is pressed
     * while the window has focus.
     */
    virtual void OnKeyPressed(KeyEventArgs& e) override;

    //Test des neuen Event Systems
    bool OnKeyF(KeyEventArgs& e);
    bool Shoot(KeyEventArgs& e);

    /**
     * Invoked when a key on the keyboard is released.
     */
    virtual void OnKeyReleased(KeyEventArgs& e);

    /**
     * Invoked when the mouse is moved over the registered window.
     */
    virtual void OnMouseMoved(MouseMotionEventArgs& e);

    /**
     * Invoked when the mouse wheel is scrolled while the registered window has focus.
     */
    virtual void OnMouseWheel(MouseWheelEventArgs& e) override;

    void RescaleHDRRenderTarget(float scale);
    virtual void OnResize(ResizeEventArgs& e) override;


    void OnGUI();

  private:
    // Some geometry to render.
    Mesh* m_CubeMesh;
    Mesh* m_SphereMesh;
    Mesh* m_ConeMesh;
    Mesh* m_TorusMesh;
    Mesh* m_PlaneMesh;

    Mesh* m_DebugCube;

    Mesh* m_SkyboxMesh;
    //std::unique_ptr<Mesh> m_SkyboxMesh;

    //TextureDx12 m_DefaultTexture;
    Texture* m_DefaultTexture;
    Texture* m_DirectXTexture;
    Texture* m_EarthTexture;
    Texture* m_MonaLisaTexture;
    Texture* m_GraceCathedralTexture;
    Reference<Texture> m_GraceCathedralCubemap;

    // HDR Render target
    std::unique_ptr<RenderTarget> m_HDRRenderTarget;

    Reference<Texture> _HDRTexture;
    Reference<Texture> _DepthTexture;

    // Root signatures
    NotCopyableReference<RenderProgramSignature> m_SkyboxSignature;
    NotCopyableReference<RenderProgramSignature> m_HDRRootSignature;
    NotCopyableReference<RenderProgramSignature> m_SDRRootSignature;

    // Pipeline state object.
    // Skybox PSO
    Reference<Kame::RenderProgram> _SkyboxProgram;
    Reference<Kame::RenderProgram> _HDRProgram;
    Reference<Kame::RenderProgram> _SDRProgram;
    //Microsoft::WRL::ComPtr<ID3D12PipelineState> m_HDRPipelineState;
    // HDR -> SDR tone mapping PSO.
    //Microsoft::WRL::ComPtr<ID3D12PipelineState> m_SDRPipelineState;

    D3D12_RECT m_ScissorRect;

    Camera m_Camera;
    struct alignas(16) CameraData {
      Kame::Math::Vector4 m_InitialCamPos;
      Kame::Math::Vector4 m_InitialCamRot;
      float m_InitialFov;
    };
    CameraData* m_pAlignedCameraData;

    // Camera controller
    float m_Forward;
    float m_Backward;
    float m_Left;
    float m_Right;
    float m_Up;
    float m_Down;

    float m_Pitch;
    float m_Yaw;

    // Rotate the lights in a circle.
    bool m_AnimateLights;
    // Set to true if the Shift key is pressed.
    bool m_Shift;

    int m_Width;
    int m_Height;

    // Scale the HDR render target to a fraction of the window size.
    float m_RenderScale;

    // Define some lights.
    std::vector<PointLight> m_PointLights;
    std::vector<SpotLight> m_SpotLights;
  };

}