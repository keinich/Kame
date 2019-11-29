#pragma once

namespace Kame {

  class Texture;

  class CommandList {

  public:

    virtual void ClearTexture(const Texture* texture, const float clearColor[4]) = 0;

  };

}