#include "kmpch.h"
#include "TextureManager.h"
#include "GraphicsCore.h"

namespace Kame {

  TextureManager* TextureManager::s_Instance = nullptr;

  TextureManager* TextureManager::Get() {
    assert(s_Instance);
    return s_Instance;
  }

  void TextureManager::Create() {
    assert(!s_Instance);

    s_Instance = new TextureManager();
  }

  void TextureManager::Destroy() {
    delete s_Instance;
    s_Instance = nullptr;
  }

  TextureManager::~TextureManager() {
    _TextureByIdentifier.clear();
  }

  Texture* TextureManager::GetTexture(std::wstring fileName, TextureUsage textureUsage) {

    auto managedMeshPair = GetOrCreateTexture(fileName);

    Texture* tex = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return tex;
    }

    tex->LoadFromFile(fileName, textureUsage);
    return tex;
  }

  std::pair<Texture*, bool> Kame::TextureManager::GetOrCreateTexture(std::wstring identifier) {
    assert(s_Instance);

    static std::mutex s_Mutex;
    std::lock_guard<std::mutex> guard(s_Mutex);

    auto iter = s_Instance->_TextureByIdentifier.find(identifier);

    // If it's found, it has already been loaded or the load process has begun
    if (iter != s_Instance->_TextureByIdentifier.end()) {
      return std::make_pair(iter->second.get(), false);
    }

    Texture* newTexture = GraphicsCore::CreateTexture();
    s_Instance->_TextureByIdentifier[identifier].reset(newTexture);

    // This was the first time it was requested, so indicate that the caller must read the file
    //return make_pair(NewTexture, true);
    return std::make_pair(newTexture, true);

  }
}
