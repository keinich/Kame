#include "kmpch.h"
#include "ToneMapping.h"
namespace Kame {
  
  ToneMapping::TonemapParameters1 g_ToneMapParameters;

  ToneMapping::TonemapParameters1 ToneMapping::GetParameters1() {
    return g_ToneMapParameters;
  }

}