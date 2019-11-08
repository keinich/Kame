#include "kmpch.h"
#include "PipelineStateManager.h"

Kame::PipelineStateManager* Kame::PipelineStateManager::_Instance;

void Kame::PipelineStateManager::DestroyAll() {
  _GraphicsPSOHashMap.clear();
}
