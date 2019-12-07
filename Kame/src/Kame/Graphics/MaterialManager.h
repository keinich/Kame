#pragma once

#include <map>
#include "Material.h"
#include <Kame/Core/References.h>
#include <string>

namespace Kame {

  class MaterialManager {

  public:

    static void Create();
    static void Destroy();

    virtual ~MaterialManager();

    template<class TMaterial>
    static TMaterial* GetMaterial();

  protected: // Methods

  protected: // Fields

    static MaterialManager* s_Instance;

    std::map<const char*, NotCopyableReference<MaterialBase>> _MaterialByIdentifier;

  };


  template<class TMaterial>
  inline TMaterial* MaterialManager::GetMaterial() {
    assert(s_Instance);

    static std::mutex s_Mutex;
    std::lock_guard<std::mutex> guard(s_Mutex);

    const char* identifier = typeid(TMaterial).name();

    auto iter = s_Instance->_MaterialByIdentifier.find(identifier);

    // If it's found, it has already been loaded or the load process has begun
    if (iter != s_Instance->_MaterialByIdentifier.end()) {
      return  static_cast<TMaterial*>(iter->second.get());
    }

    TMaterial* newMat = new TMaterial();
    s_Instance->_MaterialByIdentifier[identifier].reset(newMat);

    // This was the first time it was requested, so indicate that the caller must read the file
    //return make_pair(NewTexture, true);
    return newMat;

  }
  
}