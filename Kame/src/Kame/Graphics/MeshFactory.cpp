#include "kmpch.h"
#include "MeshFactory.h"
#include "MeshManager.h"
#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h> 

namespace Kame {

  using namespace DirectX; //for the Math

  Mesh* MeshFactory::GetCube(float size, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultCube");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateCube(managedMesh, 1, false );
    return managedMesh;
  }

  void MeshFactory::CreateCube(Mesh* mesh, float size, bool rhcoords) {
    // A cube has six faces, each one pointing in a different direction.
    const int FaceCount = 6;

    static const DirectX::XMVECTORF32 faceNormals[FaceCount] =
    {
        { 0,  0,  1 },
        { 0,  0, -1 },
        { 1,  0,  0 },
        { -1,  0,  0 },
        { 0,  1,  0 },
        { 0, -1,  0 },
    };

    static const DirectX::XMVECTORF32 textureCoordinates[4] =
    {
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 0, 0 },
    };

    VertexCollection vertices;
    IndexCollection indices;

    size /= 2;

    // Create each face in turn.
    for (int i = 0; i < FaceCount; i++) {
      DirectX::XMVECTOR normal = faceNormals[i];

      // Get two vectors perpendicular both to the face normal and to each other.
      DirectX::XMVECTOR basis = (i >= 4) ? DirectX::g_XMIdentityR2 : DirectX::g_XMIdentityR1;

      DirectX::XMVECTOR side1 = DirectX::XMVector3Cross(normal, basis);
      DirectX::XMVECTOR side2 = DirectX::XMVector3Cross(normal, side1);

      // Six indices (two triangles) per face.
      size_t vbase = vertices.size();
      indices.push_back(static_cast<uint16_t>(vbase + 0));
      indices.push_back(static_cast<uint16_t>(vbase + 1));
      indices.push_back(static_cast<uint16_t>(vbase + 2));

      indices.push_back(static_cast<uint16_t>(vbase + 0));
      indices.push_back(static_cast<uint16_t>(vbase + 2));
      indices.push_back(static_cast<uint16_t>(vbase + 3));

      // Four vertices per face.
      vertices.push_back(VertexPositionNormalTexture((normal - side1 - side2) * size, normal, textureCoordinates[0]));
      vertices.push_back(VertexPositionNormalTexture((normal - side1 + side2) * size, normal, textureCoordinates[1]));
      vertices.push_back(VertexPositionNormalTexture((normal + side1 + side2) * size, normal, textureCoordinates[2]));
      vertices.push_back(VertexPositionNormalTexture((normal + side1 - side2) * size, normal, textureCoordinates[3]));
    }

    // Create the primitive object.

    auto cl = DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->GetCommandList();
    mesh->Initialize(*cl, vertices, indices, rhcoords);

    DX12Core::Get().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY)->ExecuteCommandList(cl);
    //TODO abstract commandList.InitMesh
  }

}