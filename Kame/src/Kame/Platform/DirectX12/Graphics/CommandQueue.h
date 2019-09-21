#include "kmpch.h"

namespace Kame {

  class KAME_API CommandQueue {

  public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE type);
    ~CommandQueue();

    void Create(ID3D12Device* device);

    ID3D12CommandQueue* GetCommandQueue() { return _CommandQueue.Get(); }

  private:

    ComPtr<ID3D12CommandQueue> _CommandQueue;
    D3D12_COMMAND_LIST_TYPE _Type;
  };

}