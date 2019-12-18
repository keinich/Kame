#pragma once

#include <map>
#include <string>

#include "Font.h"

namespace Kame {

  class ManagedFont : public Font {
  public:
    ManagedFont(const std::wstring& identifier) :
      _Identifier(identifier) {}

  private:
    std::wstring _Identifier;
  };

  class FontManager {

  public:

    static void Create();
    static void Destroy();

    virtual ~FontManager();

    static Font* GetFont(std::wstring fileName);

  protected: // Methods

    static FontManager* Get();
    static std::pair<Font*, bool> GetOrCreateFont(std::wstring identifier);

  protected: // Fields

    static FontManager* s_Instance;

    std::map<std::wstring, NotCopyableReference<Font>> _FontByIdentifier;

  };

}