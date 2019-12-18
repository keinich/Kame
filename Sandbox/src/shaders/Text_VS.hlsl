cbuffer FontParameters : register(b0) {
  float2 Scale;            // Scale and offset for transforming coordinates
  float2 Offset;
  float2 InvTexDim;        // Normalizes texture coordinates
  float TextSize;            // Height of text in destination pixels
  float TextScale;        // TextSize / FontHeight
  float DstBorder;        // Extra space around a glyph measured in screen space coordinates
  uint SrcBorder;            // Extra spacing around glyphs to avoid sampling neighboring glyphs
}

struct VS_OUTPUT {
  float4 Pos : SV_POSITION;    // Upper-left and lower-right coordinates in clip space
  float2 Tex : TEXCOORD0;        // Upper-left and lower-right normalized UVs
};

struct VS_INPUT {
  float2 ScreenPos : POSITION;    // Upper-left position in screen pixel coordinates
  uint4  Glyph : TEXCOORD;        // X, Y, Width, Height in texel space
};

//ConstantBuffer<FontParameters> FontParametersCB : register(b0, space0);


VS_OUTPUT main(VS_INPUT input, uint VertID : SV_VertexID) {
  //VertexShaderOutput OUT;
  //float2 uv = float2(VertexID & 1, (VertexID >> 1) & 1);
  //float2 inputPos = input.ScreenPos;
  //float2 inputPos2 = inputPos + float2(0.2f, -0.2f);


  //OUT.TexCoord = uv;

  //OUT.Position = (float4(lerp(inputPos, inputPos2, uv), 0, 1));

  //return OUT;

  //const float2 xy0 = input.ScreenPos - DstBorder;
  const float2 xy0 = input.ScreenPos - 0;
  // lowerRight.X = upperLeft.X + GlyphWidth / LineHeightInPixels * TextSizeInPixels
  //              = upperLeft.X + (TextSizeInPixels / LineHeightInPixels) * GlyphWidth
  //              = upperLeft.X + TextScale * input.Glyph.z
  // lowerRight.Y = upperLeft.Y + TextSizeInPixels

  const float2 xy1 = input.ScreenPos + DstBorder + float2(TextScale * input.Glyph.z, TextSize);
  //const float2 xy1 = input.ScreenPos + TextSize * input.Glyph.zw;
  //const float2 xy1 = input.ScreenPos + 0 + float2(input.Glyph.z, TextSize);
  //const float2 xy1 = input.ScreenPos + 0 + float2(0.2, -0.2);
  //const uint2 uv0 = input.Glyph.xy - SrcBorder;
  const uint2 uv0 = input.Glyph.xy - 0;
  //const uint2 uv1 = input.Glyph.xy + SrcBorder + input.Glyph.zw;
  const uint2 uv1 = input.Glyph.xy + 0 + input.Glyph.zw;
  
  float2 uv = float2(VertID & 1, (VertID >> 1) & 1);

  VS_OUTPUT output;
  output.Pos = float4(lerp(xy0, xy1, uv) * Scale + Offset, 0, 1);
  //output.Pos = float4(lerp(xy0, xy1, uv) * 1 + Offset, 0, 1);
  output.Tex = lerp(uv0, uv1, uv) * InvTexDim;
  //output.Tex = lerp(uv0, uv1, uv) * 1;
  //output.Tex = uv;
  return output;
}