#include "kmpch.h"
#include "CommandContext.h"
#include "CommandManager.h"
#include "ContextManager.h"
#include "DX12Core.h"

namespace Kame {

  CommandContext::CommandContext(D3D12_COMMAND_LIST_TYPE type) :
    _Type(type),
    _CurrentAllocator(nullptr),
    _CommandList(nullptr) {}

  void CommandContext::Reset() {
    _CurrentAllocator = DX12Core::_Instance->GetCommandManager()->GetQueue(_Type).RequestAllocator();
    _CommandList->Reset(_CurrentAllocator, nullptr);
  }

  CommandContext::~CommandContext() {
    if (_CommandList != nullptr)
      _CommandList->Release();
  }

  void CommandContext::Initialize() {
    DX12Core::_Instance->GetCommandManager()->CreateNewCommandList(_Type, &_CommandList, &_CurrentAllocator);
  }

  CommandContext& CommandContext::Begin(const std::wstring ID) {
    CommandContext* newContext = DX12Core::_Instance->GetContextManager()->AllocateContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
    return *newContext;
  }

}