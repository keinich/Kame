#pragma once

#include <map>
#include "RenderApi/Texture.h"
#include "RenderApi/TextureUsage.h"
#include <string>

namespace Kame {

  class ManagedTexture : public Texture {
  public:
    ManagedTexture(const std::wstring& identifier) :
      _Identifier(identifier) {}

  private:
    std::wstring _Identifier;
  };

  class TextureManager {

    friend class MeshFactory;

  public:

    static void Create();
    static void Destroy();

    virtual ~TextureManager();

    static Texture* GetTexture(std::wstring fileName, TextureUsage textureUsage = TextureUsage::Albedo);

  protected: // Methods

    static TextureManager* Get();
    static std::pair<Texture*, bool> GetOrCreateTexture(std::wstring identifier);

  protected: // Fields

    static TextureManager* s_Instance;

    std::map<std::wstring, NotCopyableReference<Texture>> _TextureByIdentifier;

  };

}