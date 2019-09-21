#include "kmpch.h"
#include "CommandQueue.h"

Kame::CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
  _Type(type),
  _CommandQueue(nullptr) {}

Kame::CommandQueue::~CommandQueue() {}

void Kame::CommandQueue::Create(ID3D12Device* device) {

  D3D12_COMMAND_QUEUE_DESC desc = {};
  desc.Type = _Type;
  desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
  desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  desc.NodeMask = 0;

  ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_CommandQueue)));

}
