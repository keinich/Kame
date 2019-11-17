#pragma once

#include <d3d12.h>

#include <mutex>
#include <map>
#include <unordered_map>
#include <vector>

namespace Kame {

  class CommandList;
  class GpuResource;

  class ResourceStateTracker {

  public:
    ResourceStateTracker();
    virtual ~ResourceStateTracker();

    void ResourceBarrier(const D3D12_RESOURCE_BARRIER& barrier);

    void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);
    void TransitionResource(const GpuResource& resource, D3D12_RESOURCE_STATES stateAfter, UINT subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

    void UAVBarrier(const GpuResource* resource = nullptr);

    void AliasBarrier(const GpuResource* resourceBefore = nullptr, const GpuResource* resourceAfter = nullptr);

    uint32_t FlushPendingResourceBarriers(CommandList& commandList);
    void FlushResourceBarriers(CommandList& commandList);

    void CommitFinalResourceStates();

    void Reset();

    static void Lock();
    static void Unlock();

    static void AddGlobalResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES state);
    static void RemoveGlobalResourceState(ID3D12Resource* resource);

  private:

    using ResourceBarriers = std::vector<D3D12_RESOURCE_BARRIER>;

    ResourceBarriers _PendingResourceBarriers;
    ResourceBarriers _ResourceBarriers;

    struct ResourceState {
      explicit ResourceState(D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON)
        : State(state) {}

      void SetSubresourceState(UINT subresource, D3D12_RESOURCE_STATES state) {
        if (subresource == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {
          State = state;
          SubresourceState.clear();
        }
        else {
          SubresourceState[subresource] = state;
        }
      }

      D3D12_RESOURCE_STATES GetSubresourceState(UINT subresource) const {
        D3D12_RESOURCE_STATES state = State;
        const auto iter = SubresourceState.find(subresource);
        if (iter != SubresourceState.end()) {
          state = iter->second;
        }
        return state;
      }
      D3D12_RESOURCE_STATES State;
      std::map<UINT, D3D12_RESOURCE_STATES> SubresourceState;
    };

    using ResourceStateMap = std::unordered_map<ID3D12Resource*, ResourceState>;

    ResourceStateMap _FinalResourceState;
    static ResourceStateMap s_GlobalResourceState;

    static std::mutex s_GlobalMutex;
    static bool s_IsLocked;
  };

}