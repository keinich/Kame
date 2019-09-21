#include "kmpch.h"
#include "CommandManager.h"

namespace Kame {
  CommandManager::CommandManager() :
  _Device(nullptr),
  _GraphicsQueue(D3D12_COMMAND_LIST_TYPE_DIRECT){
  }
  CommandManager::~CommandManager() {}
  void CommandManager::Create(ID3D12Device2* device) {
    _Device = device;

    _GraphicsQueue.Create(device);

  }
  void CommandManager::Shutdown() {}
}