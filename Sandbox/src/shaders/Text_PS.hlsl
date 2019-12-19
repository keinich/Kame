
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
  //return saturate(SignedDistanceFieldTex.Sample(LinearClampSampler, uv) * 1.0f + 0.5);
  return saturate(SignedDistanceFieldTex.Sample(LinearClampSampler, uv) * HeightRange + 0.5);
}

float GetAlpha2(float2 uv, float range) {
  return saturate(SignedDistanceFieldTex.Sample(LinearClampSampler, uv) * range + 0.5);
}

float4 main(PS_INPUT Input) : SV_Target0
{

//  float alpha = GetAlpha(Input.uv);
//if (alpha < 0.1f) {
//  discard;
//}
//  return float4(Color.rgb, 1);



float alpha1 = GetAlpha2(Input.uv, HeightRange) * Color.a;
float alpha2 = GetAlpha2(Input.uv - ShadowOffset, HeightRange * ShadowHardness) * ShadowOpacity * Color.a;
return float4(Color.rgb * alpha1, lerp(alpha2, 1, alpha1));

}