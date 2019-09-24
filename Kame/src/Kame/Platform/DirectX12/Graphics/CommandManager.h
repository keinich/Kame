#pragma once

#include "kmpch.h"

#include "CommandQueue.h"

namespace Kame {

  class KAME_API CommandManager {

  public:

    CommandManager();
    ~CommandManager();

    void Create(ID3D12Device2* device);
    void Shutdown();

    void IdleGpu();

    CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE type);
    CommandQueue& GetGraphicsQueue() { return _GraphicsQueue; }

    void WaitForFence(uint64_t fenceValue);

    ID3D12CommandQueue* GetCommandQueue() { return _GraphicsQueue.GetCommandQueue(); }

    void CreateNewCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator);

  private: // Fields

    ID3D12Device2* _Device;

    CommandQueue _GraphicsQueue;


  };

}
