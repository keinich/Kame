#include "kmpch.h"
#include "MeshManager.h"
#include "Mesh.h"

namespace Kame {

  MeshManager* MeshManager::s_Instance = nullptr;

  MeshManager* MeshManager::Get() {
    assert(s_Instance);
    return s_Instance;
  }

  void MeshManager::Create() {
    assert(!s_Instance);

    s_Instance = new MeshManager();
  }

  void MeshManager::Destroy() {
    delete s_Instance;
    s_Instance = nullptr;
  }

  MeshManager::~MeshManager() {
    _MeshByIdentifier.clear();
  }

  std::pair<ManagedMesh*, bool> Kame::MeshManager::GetOrCreateMesh(std::wstring identifier) {
    assert(s_Instance);

    static std::mutex s_Mutex;
    std::lock_guard<std::mutex> guard(s_Mutex);

    auto iter = s_Instance->_MeshByIdentifier.find(identifier);

    // If it's found, it has already been loaded or the load process has begun
    if (iter != s_Instance->_MeshByIdentifier.end()) {
      return std::make_pair(iter->second.get(), false);
    }

    ManagedMesh* newMesh = new ManagedMesh(identifier);
    s_Instance->_MeshByIdentifier[identifier].reset(newMesh);

    // This was the first time it was requested, so indicate that the caller must read the file
    //return make_pair(NewTexture, true);
    return std::make_pair(newMesh, true);

  }
}
