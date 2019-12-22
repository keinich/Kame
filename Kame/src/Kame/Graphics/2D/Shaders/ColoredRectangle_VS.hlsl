
struct VS_OUTPUT {
  float4 Color : COLOR;
  float4 Position : SV_POSITION;
};

struct VS_INPUT {
  float2 Position : POSITION;
  float2 Scale : SCALE;
  float4 Color : COLOR;
};

VS_OUTPUT main(VS_INPUT input, uint VertID : SV_VertexID) {

  const float2 xy0 = input.Position;
  const float2 xy1 = input.Position + float2(input.Scale.x, input.Scale.y);

  //const float2 xy0 = input.Position;
  //const float2 xy1 = float2(0.2f, 0.2f);

  float2 uv = float2(VertID & 1, (VertID >> 1) & 1);

  VS_OUTPUT output;

  output.Position = float4(lerp(xy0, xy1, uv), 0, 1);
  output.Color = input.Color;
  //output.Color = float4(1, 0, 0, 1);
  //output.Color = float4(input.Position.x, input.Position.y, input.Scale.x, 1);
  //output.Color = float4(input.Position.x, input.Position.x, input.Position.x, 1);
  //output.Color = float4(input.Position.y, input.Position.y, input.Position.y, 1);
  //output.Color = float4(input.Scale.x, input.Scale.x, input.Scale.x, 1);
  //output.Color = float4(input.Scale.y, input.Scale.y, input.Scale.y, 1);

  return output;
}