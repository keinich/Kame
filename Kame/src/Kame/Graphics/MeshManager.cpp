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

  Mesh* Kame::MeshManager::GetMesh(std::wstring identifier) {
    static std::mutex s_Mutex;
    std::lock_guard<std::mutex> guard(s_Mutex);

    auto iter = s_Instance->_MeshByIdentifier.find(identifier);

    // If it's found, it has already been loaded or the load process has begun
    if (iter != s_Instance->_MeshByIdentifier.end()) {
      //return make_pair(iter->second.get(), false);
      return iter->second.get();
    }

    std::unique_ptr<Mesh> newMesh;
    s_Instance->_MeshByIdentifier[identifier].reset(newMesh.get());

    // This was the first time it was requested, so indicate that the caller must read the file
    //return make_pair(NewTexture, true);
  }
}
