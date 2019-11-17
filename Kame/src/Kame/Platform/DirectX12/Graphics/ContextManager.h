//#pragma once
//
//#include "kmpch.h"
//
//#include "CommandContext.h"
//
//namespace Kame {
//
//  class ContextManager {
//
//  public:
//    ContextManager();
//
//    class CommandContext* AllocateContext(D3D12_COMMAND_LIST_TYPE type);
//    void FreeContext(CommandContext* usedContext);
//
//  private:
//
//    std::vector<std::unique_ptr<CommandContext>> _ContextPool[4];
//    std::queue<CommandContext*> _AvailableContexts[4];
//    std::mutex _ContextAllocationMutex;
//
//  };
//
//}
