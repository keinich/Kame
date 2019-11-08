#include "kmpch.h"
#include "GraphicsCommon.h"

D3D12_RASTERIZER_DESC Kame::GraphicsCommon::RasterizerDefault;

void Kame::GraphicsCommon::InitializeCommonState() {

  RasterizerDefault.FillMode = D3D12_FILL_MODE_SOLID;
  RasterizerDefault.CullMode = D3D12_CULL_MODE_BACK;
  RasterizerDefault.FrontCounterClockwise = TRUE;
  RasterizerDefault.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
  RasterizerDefault.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
  RasterizerDefault.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  RasterizerDefault.DepthClipEnable = TRUE;
  RasterizerDefault.MultisampleEnable = FALSE;
  RasterizerDefault.AntialiasedLineEnable = FALSE;
  RasterizerDefault.ForcedSampleCount = 0;
  RasterizerDefault.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

}
