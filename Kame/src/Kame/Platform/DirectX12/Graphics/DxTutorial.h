#pragma once
#include <cstdint>

namespace Kame {

  class DxTutorial {

  public:
    DxTutorial();
    ~DxTutorial();

  private:
    const uint8_t c_NumFrames = 3;
    bool _UseWarp = false;

    uint32_t _ClientWidth = 1280;
    uint32_t _ClientHeight = 720;

    bool _IsInitialized = false;

  };

}