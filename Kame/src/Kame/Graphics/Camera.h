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
  *  @file Camera.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A DirectX camera class.
  */


//#include <DirectXMath.h>
#include "Kame/Math/Vector4.h"
#include <Kame\Math\Vector4.h>
#include "Kame/Math/Matrix4x4.h"

namespace Kame {

  // When performing transformations on the camera, 
  // it is sometimes useful to express which space this 
  // transformation should be applied.
  enum class Space {
    Local,
    World,
  };

  class Camera {
  public:

    Camera();
    virtual ~Camera();

    void XM_CALLCONV set_LookAt(Kame::Math::Vector4 eye, Kame::Math::Vector4 target, Kame::Math::Vector4 up);
    Kame::Math::Matrix4x4 get_ViewMatrix() const;
    Kame::Math::Matrix4x4 get_InverseViewMatrix() const;

    /**
     * Set the camera to a perspective projection matrix.
     * @param fovy The vertical field of view in degrees.
     * @param aspect The aspect ratio of the screen.
     * @param zNear The distance to the near clipping plane.
     * @param zFar The distance to the far clipping plane.
     */
    void set_Projection(float fovy, float aspect, float zNear, float zFar);
    Kame::Math::Matrix4x4 get_ProjectionMatrix() const;
    Kame::Math::Matrix4x4 get_InverseProjectionMatrix() const;

    /**
     * Set the field of view in degrees.
     */
    void set_FoV(float fovy);

    /**
     * Get the field of view in degrees.
     */
    float get_FoV() const;

    /**
     * Set the camera's position in world-space.
     */
    void XM_CALLCONV set_Translation(Kame::Math::Vector4 translation);
    Kame::Math::Vector4 get_Translation() const;

    /**
     * Set the camera's rotation in world-space.
     * @param rotation The rotation quaternion.
     */
    void XM_CALLCONV set_Rotation(Kame::Math::Vector4 rotation);
    /**
     * Query the camera's rotation.
     * @returns The camera's rotation quaternion.
     */
    Kame::Math::Vector4 get_Rotation() const;

    void XM_CALLCONV Translate(Kame::Math::Vector4 translation, Space space = Space::Local);
    void Rotate(Kame::Math::Vector4 quaternion);

  protected:
    virtual void UpdateViewMatrix() const;
    virtual void UpdateInverseViewMatrix() const;
    virtual void UpdateProjectionMatrix() const;
    virtual void UpdateInverseProjectionMatrix() const;

    // This data must be aligned otherwise the SSE intrinsics fail
    // and throw exceptions.
    __declspec(align(16)) struct AlignedData {
      // World-space position of the camera.      
      Kame::Math::Vector4 _Translation;
      // World-space rotation of the camera.
      // THIS IS A QUATERNION!!!!      
      Kame::Math::Vector4 _Rotation;

      Kame::Math::Matrix4x4 _ViewMatrix, _InverseViewMatrix;
      Kame::Math::Matrix4x4 _ProjectionMatrix, _InverseProjectionMatrix;
    };
    AlignedData* pData;

    // projection parameters
    float _vFoV;   // Vertical field of view.
    float _AspectRatio; // Aspect ratio
    float _zNear;      // Near clip distance
    float _zFar;       // Far clip distance.

    // True if the view matrix needs to be updated.
    mutable bool _ViewDirty, _InverseViewDirty;
    // True if the projection matrix needs to be updated.
    mutable bool _ProjectionDirty, _InverseProjectionDirty;

  private:

  };

}