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

    CommandQueue& GetGraphicsQueue() { return _GraphicsQueue; }

    ID3D12CommandQueue* GetCommandQueue() { return _GraphicsQueue.GetCommandQueue(); }

  private: // Fields

    ID3D12Device2* _Device;

    CommandQueue _GraphicsQueue;


  };

}
