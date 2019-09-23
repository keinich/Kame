#pragma once

#include "kmpch.h"

namespace Kame {


  class CommandContext {

    friend class ContextManager;

  private:

    CommandContext(D3D12_COMMAND_LIST_TYPE type);

    void Reset();

  public:

    ~CommandContext();

    void Initialize();

    static CommandContext& Begin(const std::wstring ID = L"");

  private:

    ID3D12GraphicsCommandList* _CommandList;
    ID3D12CommandAllocator* _CurrentAllocator;

    D3D12_COMMAND_LIST_TYPE _Type;

  };

}
