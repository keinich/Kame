#include "kmpch.h"
#include "FontManager.h"
#include "Kame/Graphics/GraphicsCore.h"

namespace Kame {

  FontManager* FontManager::s_Instance = nullptr;

  FontManager* FontManager::Get() {
    assert(s_Instance);
    return s_Instance;
  }

  void FontManager::Create() {
    assert(!s_Instance);

    s_Instance = new FontManager();
  }

  void FontManager::Destroy() {
    delete s_Instance;
    s_Instance = nullptr;
  }

  FontManager::~FontManager() {
    _FontByIdentifier.clear();
  }

  Font* FontManager::GetFont(std::wstring fileName) {

    auto managedMeshPair = GetOrCreateFont(fileName);

    Font* font = managedMeshPair.first;
    const bool mustLoad = managedMeshPair.second;

    if (!mustLoad) {
      //ManTex->WaitForLoad(); //TODO
      return font;
    }

    font->Load(fileName);
    return font;
  }

  std::pair<Font*, bool> Kame::FontManager::GetOrCreateFont(std::wstring identifier) {
    assert(s_Instance);

    static std::mutex s_Mutex;
    std::lock_guard<std::mutex> guard(s_Mutex);

    auto iter = s_Instance->_FontByIdentifier.find(identifier);

    // If it's found, it has already been loaded or the load process has begun
    if (iter != s_Instance->_FontByIdentifier.end()) {
      return std::make_pair(iter->second.get(), false);
    }

    Font* newFont = new Font();
    s_Instance->_FontByIdentifier[identifier].reset(newFont);

    // This was the first time it was requested, so indicate that the caller must read the file
    //return make_pair(NewFont, true);
    return std::make_pair(newFont, true);

  }
}
