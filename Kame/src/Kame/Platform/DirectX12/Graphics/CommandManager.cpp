#include "kmpch.h"
#include "CommandManager.h"

namespace Kame {
  CommandManager::CommandManager() :
    _Device(nullptr),
    _GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT) {}
  CommandManager::~CommandManager() {}
  void CommandManager::Create(ID3D12Device2* device) {
    _Device = device;

    _GraphicsQueue.Create(device);

  }
  void CommandManager::Shutdown() {
    _GraphicsQueue.Shutdown();
  }
  void CommandManager::IdleGpu() {
    _GraphicsQueue.WaitForIdle();
  }

  CommandQueue& CommandManager::GetQueue(D3D12_COMMAND_LIST_TYPE type) {
    switch (type) {    
    case D3D12_COMMAND_LIST_TYPE_COMPUTE: throw; // TODO
    case D3D12_COMMAND_LIST_TYPE_COPY: throw; // TODO
    default: return _GraphicsQueue;
    }
  }
  void CommandManager::CreateNewCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator) {

    switch (type) {
    case D3D12_COMMAND_LIST_TYPE_DIRECT: 
      *allocator = _GraphicsQueue.RequestAllocator(); 
      break;
    }

    ThrowIfFailed(_Device->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list)));

  }
}