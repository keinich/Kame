#pragma once
#include <string>
#include <cassert>
#include <vector>

namespace Kame {

  enum class ShaderDataType {
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
  };

  static uint32_t ShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::Float:
      return 4;
    case ShaderDataType::Float2:
      return 4 * 2;
    case ShaderDataType::Float3:
      return 4 * 3;
    case ShaderDataType::Float4:
      return 4 * 4;
    case ShaderDataType::Mat3:
      return 4 * 3 * 3;
    case ShaderDataType::Mat4:
      return 4 * 4 * 4;
    case ShaderDataType::Int:
      return 4;
    case ShaderDataType::Int2:
      return 4 * 2;
    case ShaderDataType::Int3:
      return 4 * 3;
    case ShaderDataType::Int4:
      return 4 * 4;
    }

    assert(false);
    return 0;
  }

  struct VertexElement {
    std::string Name;
    ShaderDataType Type;
    uint32_t Offset;
    uint32_t Size;

    VertexElement(ShaderDataType type, const std::string& name) :
      Name(name),
      Type(type),
      Size(ShaderDataTypeSize(type)),
      Offset(0) {

    }
  };

  class VertexLayout {

  public:

    VertexLayout(const std::initializer_list<VertexElement>& elements) :
      _Elements(elements) {
      CalculateOffestsAndStride();
    }

    inline const std::vector<VertexElement>& GetElements() const { return _Elements; }

    std::vector<VertexElement>::iterator begin() { return _Elements.begin(); }
    std::vector<VertexElement>::iterator end() { return _Elements.begin(); }

  private: // Methods
    void CalculateOffestsAndStride() {
      uint32_t offset = 0;
      _Stride = 0;
      for (auto& element : _Elements) {
        element.Offset = offset;
        offset += element.Size;
        _Stride += element.Size;
      }
    }

  private: // Fields

    std::vector<VertexElement> _Elements;
    uint32_t _Stride = 0;
  };

}