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
  *  @file Mesh.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief A mesh class encapsulates the index and vertex buffers for a geometric primitive.
  */

  //TODO make this RenderApi independent
#include "Kame/Graphics/RenderApi/CommandList.h"
#include "Kame/Platform/DirectX12/Graphics/CommandListDx12.h"
#include "Kame/Platform/DirectX12/Graphics/VertexBufferDx12.h"
#include "Kame/Platform/DirectX12/Graphics/IndexBufferDx12.h"

#include <DirectXMath.h>
#include <d3d12.h>

#include <wrl.h>

#include <memory> // For std::unique_ptr
#include <vector>

namespace Kame {

  // Vertex struct holding position, normal vector, and texture mapping information.
  struct KAME_API VertexPositionNormalTexture {
    VertexPositionNormalTexture() {}

    VertexPositionNormalTexture(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT2& textureCoordinate)
      : position(position),
      normal(normal),
      textureCoordinate(textureCoordinate) {}

    VertexPositionNormalTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR textureCoordinate) {
      XMStoreFloat3(&this->position, position);
      XMStoreFloat3(&this->normal, normal);
      XMStoreFloat2(&this->textureCoordinate, textureCoordinate);
    }

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 textureCoordinate;

    static const int InputElementCount = 3;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
  };

  // Vertex struct holding position, normal vector, and texture mapping information.
  struct KAME_API VertexPosition {
    VertexPosition() {}

    VertexPosition(const DirectX::XMFLOAT3& position)
      : position(position) {}

    VertexPosition(DirectX::FXMVECTOR position) {
      XMStoreFloat3(&this->position, position);
    }

    DirectX::XMFLOAT3 position;

    static const int InputElementCount = 1;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
  };

  using VertexCollection = std::vector<VertexPositionNormalTexture>;
  using IndexCollection = std::vector<uint16_t>;

  using SimpleVertexCollection = std::vector<VertexPosition>;

  class KAME_API Mesh {

    friend class MeshManager;

  public:

    void Draw(CommandList* commandList);

    static std::unique_ptr<Mesh> CreateCube(CommandListDx12& commandList, float size = 1, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateSphere(CommandListDx12& commandList, float diameter = 1, size_t tessellation = 16, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateCone(CommandListDx12& commandList, float diameter = 1, float height = 1, size_t tessellation = 32, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreateTorus(CommandListDx12& commandList, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32, bool rhcoords = false);
    static std::unique_ptr<Mesh> CreatePlane(CommandListDx12& commandList, float width = 1, float height = 1, bool rhcoords = false);

    static std::unique_ptr<Mesh> CreateDebugCube(CommandListDx12& commandList, float size = 1, bool rhcoords = false);

  protected:

  private:
    friend struct std::default_delete<Mesh>;

    Mesh();
    Mesh(const Mesh& copy) = delete;
    virtual ~Mesh();

    void Initialize(CommandListDx12& commandList, VertexCollection& vertices, IndexCollection& indices, bool rhcoords);
    void Initialize(CommandListDx12& commandList, SimpleVertexCollection& vertices, IndexCollection& indices, bool rhcoords);

    VertexBufferDx12 m_VertexBuffer;
    IndexBufferDx12 m_IndexBuffer;

    UINT m_IndexCount;
  };

}