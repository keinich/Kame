#include "kmpch.h"
//#include "GraphicsContext.h"
//#include "ColorBuffer.h"
//#include "DepthBuffer.h"
//#include "UploadBuffer.h"
//
//namespace Kame {
//
//  void GraphicsContext::ClearColor(D3D12_CPU_DESCRIPTOR_HANDLE rtv, float* color) {
//    _CommandList->ClearRenderTargetView(rtv, color, 0, nullptr);
//  }
//
//  void GraphicsContext::ClearColor(ColorBuffer& target, float* color) {
//    //TransitionBarrier(target, D3D12_RESOURCE_STATE_RENDER_TARGET);
//    _CommandList->ClearRenderTargetView(target.GetRtv(), color, 0, nullptr);
//
//    //TrackResource(target);
//  }
//
//  void GraphicsContext::ClearDepth(DepthBuffer& target) {
//    _CommandList->ClearDepthStencilView(target.GetDsv(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//  }
//
//  void GraphicsContext::SetRenderTargets(UINT numRtvs, D3D12_CPU_DESCRIPTOR_HANDLE rtvs[], D3D12_CPU_DESCRIPTOR_HANDLE dsv) {
//    _CommandList->OMSetRenderTargets(numRtvs, rtvs, FALSE, &dsv);
//  }
//
//  void GraphicsContext::SetGraphicsDynamicConstantBuffer(uint32_t rootParameterIndex, size_t sizeInBytes, const void* bufferData) {
//    auto heapAllocation = _UploadBuffer->Allocate(sizeInBytes);
//    memcpy(heapAllocation.Cpu, bufferData, sizeInBytes);
//    _CommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, heapAllocation.Gpu);
//  }
//
//
//}
