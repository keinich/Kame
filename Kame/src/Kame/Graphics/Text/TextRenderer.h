#pragma once

#include <Kame/Core/References.h>
#include <Kame/Math/Vector4.h>
#include "TextRenderItem.h"

namespace Kame {

  class RenderProgramSignature;
  class RenderProgram;
  class CommandList;
  class Texture; //TODO Remove this
  class Font;
  class TextRenderContext;

  __declspec(align(16)) struct TextVert {
    float X, Y;                // Upper-left glyph position in screen space
    uint16_t U, V, W, H;    // Upper-left glyph UV and the width in texture space
  };

  class TextRenderer {

    friend class TextRenderContext;

  public:

    static void Create();
    static void Destroy();

    inline static TextRenderer* Get() { return s_Instance; }

    void Initialize();

    void RenderTextItems(CommandList* commandList, std::vector<Reference<TextRenderItem>>& textRenderItems);

  protected: // Methods
    TextRenderer();
    virtual ~TextRenderer();

    void Begin(CommandList* commandList);

  private: // Fields

    static TextRenderer* s_Instance;



    NotCopyableReference<RenderProgramSignature> _ProgramSignature;
    NotCopyableReference<RenderProgram> _Program;

  };

}