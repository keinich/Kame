#include "kmpch.h"
#include "MeshFactory.h"
#include "MeshManager.h"
#include <Kame/Platform/DirectX12/Graphics/DX12Core.h>
#include <Kame/Platform/DirectX12/Graphics/CommandQueue.h> 

namespace Kame {

  using namespace DirectX; //for the Math

  Mesh* MeshFactory::GetCube(float size, bool rhcoords) {
    std::wstring name = L"KameDefaultCube";
    std::wstring sizeName = std::to_wstring(size);
    std::wstring rhcoordsName = std::to_wstring(rhcoords);
    std::wstring fullName = name + sizeName + rhcoordsName;

    auto managedMeshPair = MeshManager::GetOrCreateMesh(fullName);

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateCube(managedMesh, size, rhcoords);
    return managedMesh;
  }

  Mesh* MeshFactory::GetSphere(float diameter, size_t tessellation, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultSphere");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateSphere(managedMesh, diameter, tessellation, rhcoords);
    return managedMesh;
  }

  Mesh* MeshFactory::GetCone(float diameter, float height, size_t tessellation, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultCone");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateCone(managedMesh, diameter, height, tessellation, rhcoords);
    return managedMesh;;
  }

  Mesh* MeshFactory::GetTorus(float diameter, float thickness, size_t tessellation, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultTorus");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateTorus(managedMesh, diameter, thickness, tessellation, rhcoords);
    return managedMesh;
  }

  Mesh* MeshFactory::GetPlane(float width, float height, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultPlane");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreatePlane(managedMesh, width, height, rhcoords);
    return managedMesh;
  }

  Mesh* MeshFactory::GetDebugCube(float size, bool rhcoords) {
    auto managedMeshPair = MeshManager::GetOrCreateMesh(L"KameDefaultDebugCube");

    ManagedMesh* managedMesh = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return managedMesh;
    }

    CreateDebugCube(managedMesh, size, rhcoords);
    return managedMesh;
  }

#pragma region " Create Functions "

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
    mesh->Initialize(vertices, indices, rhcoords);

  }

  void MeshFactory::CreateSphere(Mesh* mesh, float diameter, size_t tessellation, bool rhcoords) {
    VertexCollection vertices;
    IndexCollection indices;

    if (tessellation < 3)
      throw std::out_of_range("tessellation parameter out of range");

    float radius = diameter / 2.0f;
    size_t verticalSegments = tessellation;
    size_t horizontalSegments = tessellation * 2;

    // Create rings of vertices at progressively higher latitudes.
    for (size_t i = 0; i <= verticalSegments; i++) {
      float v = 1 - (float)i / verticalSegments;

      float latitude = (i * XM_PI / verticalSegments) - XM_PIDIV2;
      float dy, dxz;

      XMScalarSinCos(&dy, &dxz, latitude);

      // Create a single ring of vertices at this latitude.
      for (size_t j = 0; j <= horizontalSegments; j++) {
        float u = (float)j / horizontalSegments;

        float longitude = j * XM_2PI / horizontalSegments;
        float dx, dz;

        XMScalarSinCos(&dx, &dz, longitude);

        dx *= dxz;
        dz *= dxz;

        XMVECTOR normal = XMVectorSet(dx, dy, dz, 0);
        XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

        vertices.push_back(VertexPositionNormalTexture(normal * radius, normal, textureCoordinate));
      }
    }

    // Fill the index buffer with triangles joining each pair of latitude rings.
    size_t stride = horizontalSegments + 1;

    for (size_t i = 0; i < verticalSegments; i++) {
      for (size_t j = 0; j <= horizontalSegments; j++) {
        size_t nextI = i + 1;
        size_t nextJ = (j + 1) % stride;

        indices.push_back(static_cast<uint16_t>(i * stride + j));
        indices.push_back(static_cast<uint16_t>(nextI * stride + j));
        indices.push_back(static_cast<uint16_t>(i * stride + nextJ));

        indices.push_back(static_cast<uint16_t>(i * stride + nextJ));
        indices.push_back(static_cast<uint16_t>(nextI * stride + j));
        indices.push_back(static_cast<uint16_t>(nextI * stride + nextJ));
      }
    }

    // Create the primitive object.
    mesh->Initialize(vertices, indices, rhcoords);
  }

  // Helper computes a point on a unit circle, aligned to the x/z plane and centered on the origin.
  static inline XMVECTOR GetCircleVector(size_t i, size_t tessellation) {
    float angle = i * XM_2PI / tessellation;
    float dx, dz;

    XMScalarSinCos(&dx, &dz, angle);

    XMVECTORF32 v = { dx, 0, dz, 0 };
    return v;
  }

  static inline XMVECTOR GetCircleTangent(size_t i, size_t tessellation) {
    float angle = (i * XM_2PI / tessellation) + XM_PIDIV2;
    float dx, dz;

    XMScalarSinCos(&dx, &dz, angle);

    XMVECTORF32 v = { dx, 0, dz, 0 };
    return v;
  }

  // Helper creates a triangle fan to close the end of a cylinder / cone
  static void CreateCylinderCap(VertexCollection& vertices, IndexCollection& indices, size_t tessellation, float height, float radius, bool isTop) {
    // Create cap indices.
    for (size_t i = 0; i < tessellation - 2; i++) {
      size_t i1 = (i + 1) % tessellation;
      size_t i2 = (i + 2) % tessellation;

      if (isTop) {
        std::swap(i1, i2);
      }

      size_t vbase = vertices.size();
      indices.push_back(static_cast<uint16_t>(vbase));
      indices.push_back(static_cast<uint16_t>(vbase + i1));
      indices.push_back(static_cast<uint16_t>(vbase + i2));
    }

    // Which end of the cylinder is this?
    XMVECTOR normal = g_XMIdentityR1;
    XMVECTOR textureScale = g_XMNegativeOneHalf;

    if (!isTop) {
      normal = -normal;
      textureScale *= g_XMNegateX;
    }

    // Create cap vertices.
    for (size_t i = 0; i < tessellation; i++) {
      XMVECTOR circleVector = GetCircleVector(i, tessellation);

      XMVECTOR position = (circleVector * radius) + (normal * height);

      XMVECTOR textureCoordinate = XMVectorMultiplyAdd(XMVectorSwizzle<0, 2, 3, 3>(circleVector), textureScale, g_XMOneHalf);

      vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));
    }
  }

  void MeshFactory::CreateCone(Mesh* mesh, float diameter, float height, size_t tessellation, bool rhcoords) {
    VertexCollection vertices;
    IndexCollection indices;

    if (tessellation < 3)
      throw std::out_of_range("tessellation parameter out of range");

    height /= 2;

    XMVECTOR topOffset = g_XMIdentityR1 * height;

    float radius = diameter / 2;
    size_t stride = tessellation + 1;

    // Create a ring of triangles around the outside of the cone.
    for (size_t i = 0; i <= tessellation; i++) {
      XMVECTOR circlevec = GetCircleVector(i, tessellation);

      XMVECTOR sideOffset = circlevec * radius;

      float u = (float)i / tessellation;

      XMVECTOR textureCoordinate = XMLoadFloat(&u);

      XMVECTOR pt = sideOffset - topOffset;

      XMVECTOR normal = XMVector3Cross(GetCircleTangent(i, tessellation), topOffset - pt);
      normal = XMVector3Normalize(normal);

      // Duplicate the top vertex for distinct normals
      vertices.push_back(VertexPositionNormalTexture(topOffset, normal, g_XMZero));
      vertices.push_back(VertexPositionNormalTexture(pt, normal, textureCoordinate + g_XMIdentityR1));

      indices.push_back(static_cast<uint16_t>(i * 2));
      indices.push_back(static_cast<uint16_t>((i * 2 + 3) % (stride * 2)));
      indices.push_back(static_cast<uint16_t>((i * 2 + 1) % (stride * 2)));
    }

    // Create flat triangle fan caps to seal the bottom.
    CreateCylinderCap(vertices, indices, tessellation, height, radius, false);

    // Create the primitive object.
    mesh->Initialize(vertices, indices, rhcoords);    
  }

  void MeshFactory::CreateTorus(Mesh* mesh, float diameter, float thickness, size_t tessellation, bool rhcoords) {
    VertexCollection vertices;
    IndexCollection indices;

    if (tessellation < 3)
      throw std::out_of_range("tessellation parameter out of range");

    size_t stride = tessellation + 1;

    // First we loop around the main ring of the torus.
    for (size_t i = 0; i <= tessellation; i++) {
      float u = (float)i / tessellation;

      float outerAngle = i * XM_2PI / tessellation - XM_PIDIV2;

      // Create a transform matrix that will align geometry to
      // slice perpendicularly though the current ring position.
      XMMATRIX transform = XMMatrixTranslation(diameter / 2, 0, 0) * XMMatrixRotationY(outerAngle);

      // Now we loop along the other axis, around the side of the tube.
      for (size_t j = 0; j <= tessellation; j++) {
        float v = 1 - (float)j / tessellation;

        float innerAngle = j * XM_2PI / tessellation + XM_PI;
        float dx, dy;

        XMScalarSinCos(&dy, &dx, innerAngle);

        // Create a vertex.
        XMVECTOR normal = XMVectorSet(dx, dy, 0, 0);
        XMVECTOR position = normal * thickness / 2;
        XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

        position = XMVector3Transform(position, transform);
        normal = XMVector3TransformNormal(normal, transform);

        vertices.push_back(VertexPositionNormalTexture(position, normal, textureCoordinate));

        // And create indices for two triangles.
        size_t nextI = (i + 1) % stride;
        size_t nextJ = (j + 1) % stride;

        indices.push_back(static_cast<uint16_t>(i * stride + j));
        indices.push_back(static_cast<uint16_t>(i * stride + nextJ));
        indices.push_back(static_cast<uint16_t>(nextI * stride + j));

        indices.push_back(static_cast<uint16_t>(i * stride + nextJ));
        indices.push_back(static_cast<uint16_t>(nextI * stride + nextJ));
        indices.push_back(static_cast<uint16_t>(nextI * stride + j));
      }
    }

    // Create the primitive object.
    mesh->Initialize(vertices, indices, rhcoords);   
  }

  void MeshFactory::CreatePlane(Mesh* mesh, float width, float height, bool rhcoords) {
    VertexCollection vertices =
    {
        { XMFLOAT3(-0.5f * width, 0.0f,  0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // 0
        { XMFLOAT3(0.5f * width, 0.0f,  0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) }, // 1
        { XMFLOAT3(0.5f * width, 0.0f, -0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) }, // 2
        { XMFLOAT3(-0.5f * width, 0.0f, -0.5f * height), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) }  // 3
    };

    IndexCollection indices =
    {
        0, 3, 1, 1, 3, 2
    };

    mesh->Initialize(vertices, indices, rhcoords);
  }

  void MeshFactory::CreateDebugCube(Mesh* mesh, float size, bool rhcoords) {
    // A cube has six faces, each one pointing in a different direction.
    const int FaceCount = 6;

    SimpleVertexCollection vertices;
    IndexCollection indices;

    size /= 2;

    static const XMVECTORF32 faceNormals[FaceCount] =
    {
        { 0,  0,  1 },
        { 0,  0, -1 },
        { 1,  0,  0 },
        { -1,  0,  0 },
        { 0,  1,  0 },
        { 0, -1,  0 },
    };

    static const XMVECTORF32 textureCoordinates[4] =
    {
        { 1, 0 },
        { 1, 1 },
        { 0, 1 },
        { 0, 0 },
    };

    // Create each face in turn.
    for (int i = 0; i < FaceCount; i++) {
      XMVECTOR normal = faceNormals[i];

      // Get two vectors perpendicular both to the face normal and to each other.
      XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;

      XMVECTOR side1 = XMVector3Cross(normal, basis);
      XMVECTOR side2 = XMVector3Cross(normal, side1);

      // Six indices (two triangles) per face.
      size_t vbase = vertices.size();
      indices.push_back(static_cast<uint16_t>(vbase + 0));
      indices.push_back(static_cast<uint16_t>(vbase + 1));
      indices.push_back(static_cast<uint16_t>(vbase + 2));

      indices.push_back(static_cast<uint16_t>(vbase + 0));
      indices.push_back(static_cast<uint16_t>(vbase + 2));
      indices.push_back(static_cast<uint16_t>(vbase + 3));

      // Four vertices per face.
      vertices.push_back(VertexPosition((normal - side1 - side2) * size));
      vertices.push_back(VertexPosition((normal - side1 + side2) * size));
      vertices.push_back(VertexPosition((normal + side1 + side2) * size));
      vertices.push_back(VertexPosition((normal + side1 - side2) * size));
    }

    // Create the primitive object.
    mesh->Initialize(vertices, indices, rhcoords);
  }

  // Helper for flipping winding of geometric primitives for LH vs. RH coords
  static void ReverseWinding(IndexCollection& indices, VertexCollection& vertices) {
    assert((indices.size() % 3) == 0);
    for (auto it = indices.begin(); it != indices.end(); it += 3) {
      std::swap(*it, *(it + 2));
    }

    for (auto it = vertices.begin(); it != vertices.end(); ++it) {
      it->textureCoordinate.x = (1.f - it->textureCoordinate.x);
    }
  }

#pragma endregion

}