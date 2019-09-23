#include "kmpch.h"
#include "ContextManager.h"

namespace Kame {

  ContextManager::ContextManager() {}

  CommandContext* ContextManager::AllocateContext(D3D12_COMMAND_LIST_TYPE type) {

    std::lock_guard<std::mutex> LockGuard(_ContextAllocationMutex);

    auto& availableContexts = _AvailableContexts[type];

    CommandContext* ret = nullptr;
    if (availableContexts.empty()) {
      ret = new CommandContext(type);
      _ContextPool[type].emplace_back(ret);
      ret->Initialize();
    }
    else {
      ret = availableContexts.front();
      availableContexts.pop();
      ret->Reset();
    }

    assert(ret != nullptr);
    assert(ret->_Type == type);
    
    return ret;

  }



}

