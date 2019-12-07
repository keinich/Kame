#include "kmpch.h"
#include "MaterialManager.h"

namespace Kame {

  MaterialManager* MaterialManager::s_Instance = nullptr;

  void MaterialManager::Create() {
    s_Instance = new MaterialManager();
  }
  void MaterialManager::Destroy() {
    delete s_Instance;
    s_Instance = nullptr;
  }

  MaterialManager::~MaterialManager() {
    _MaterialByIdentifier.clear();
  }

}
