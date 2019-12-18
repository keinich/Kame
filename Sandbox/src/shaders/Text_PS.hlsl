
cbuffer FontParameters : register(b0) {
  float4 Color;
  float2 ShadowOffset;
  float ShadowHardness;
  float ShadowOpacity;
  float HeightRange;    // The range of the signed distance field.
};

//ConstantBuffer<FontParameters> FontParametersCB : register(b0);

Texture2D<float3> SignedDistanceFieldTex : register(t0);
SamplerState LinearClampSampler : register(s0);

//ConstantBuffer<FontParameters> FontParametersCB : register(b0, space1);

struct PS_INPUT {
  float4 pos : SV_POSITION;
  float2 uv : TEXCOORD0;
};

float GetAlpha(float2 uv) {
  return saturate(SignedDistanceFieldTex.Sample(LinearClampSampler, uv) * 1.0f + 0.5);
  //return saturate(SignedDistanceFieldTex.Sample(LinearClampSampler, uv) * HeightRange + 0.5);
}

float4 main(PS_INPUT Input) : SV_Target0
{
  //float3 HDR = SignedDistanceFieldTex.SampleLevel(LinearClampSampler, TexCoord, 0);

  // Add exposure to HDR result.
  //HDR *= exp2(TonemapParametersCB.Exposure);

  float alpha = GetAlpha(Input.uv);

//if (alpha <= 0.2f) {
//  discard;
//}
return float4(Color.rgb, 1) * GetAlpha(Input.uv) * Color.a;
  //return float4(alpha, alpha, alpha, alpha);
  //return float4(SignedDistanceFieldTex.SampleLevel(LinearClampSampler, Input.uv, 0), 1);
  //return float4(Color.rgb, 1);

  //return float4(Color.rgb, 1) * GetAlpha(Input.uv) * Color.a;


  //return float4(1, 0, 0, 1);
  //return Color * float4(HDR, 1);
  //return float4(HDR, 1);

}