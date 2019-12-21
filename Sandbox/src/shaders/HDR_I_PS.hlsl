struct PixelShaderInput {
  float4 PositionVS : POSITION;
  float3 NormalVS   : NORMAL;
  float2 TexCoord   : TEXCOORD;
  nointerpolation uint MatIndex  : MATINDEX;
};

struct Material {
  float4 Emissive;
  //----------------------------------- (16 byte boundary)
  float4 Ambient;
  //----------------------------------- (16 byte boundary)
  float4 Diffuse;
  //----------------------------------- (16 byte boundary)
  float4 Specular;
  //----------------------------------- (16 byte boundary)
  float  SpecularPower;
  float3 Padding;
  //----------------------------------- (16 byte boundary)
  // Total:                              16 * 5 = 80 bytes
};

struct MaterialParameters {
  float4 Emissive;
  //----------------------------------- (16 byte boundary)
  float4 Ambient;
  //----------------------------------- (16 byte boundary)
  float4 Diffuse;
  //----------------------------------- (16 byte boundary)
  float4 Specular;
  //----------------------------------- (16 byte boundary)
  float  SpecularPower;
  uint TextureIndex;
  float2 Padding;
  //----------------------------------- (16 byte boundary)
  // Total:                              16 * 5 = 80 bytes
  
};

struct PointLight {
  float4 PositionWS; // Light position in world space.
  //----------------------------------- (16 byte boundary)
  float4 PositionVS; // Light position in view space.
  //----------------------------------- (16 byte boundary)
  float4 Color;
  //----------------------------------- (16 byte boundary)
  float       Intensity;
  float       Attenuation;
  float2      Padding;                // Pad to 16 bytes
  //----------------------------------- (16 byte boundary)
  // Total:                              16 * 4 = 64 bytes
};

struct SpotLight {
  float4 PositionWS; // Light position in world space.
  //----------------------------------- (16 byte boundary)
  float4 PositionVS; // Light position in view space.
  //----------------------------------- (16 byte boundary)
  float4 DirectionWS; // Light direction in world space.
  //----------------------------------- (16 byte boundary)
  float4 DirectionVS; // Light direction in view space.
  //----------------------------------- (16 byte boundary)
  float4 Color;
  //----------------------------------- (16 byte boundary)
  float       Intensity;
  float       SpotAngle;
  float       Attenuation;
  float       Padding;                // Pad to 16 bytes.
  //----------------------------------- (16 byte boundary)
  // Total:                              16 * 6 = 96 bytes
};

struct LightProperties {
  uint NumPointLights;
  uint NumSpotLights;
};

struct LightResult {
  float4 Diffuse;
  float4 Specular;
};

ConstantBuffer<Material> MaterialCB : register(b0, space1);
ConstantBuffer<LightProperties> LightPropertiesCB : register(b1);

StructuredBuffer<PointLight> PointLights : register(t0);
StructuredBuffer<SpotLight> SpotLights : register(t1);
Texture2D DiffuseTexture[]            : register(t2);

SamplerState LinearRepeatSampler    : register(s0);

StructuredBuffer<MaterialParameters> materialParameters : register(t1, space1);

float3 LinearToSRGB(float3 x) {
  // This is exactly the sRGB curve
  //return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;

  // This is cheaper but nearly equivalent
  return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(abs(x - 0.00228)) - 0.13448 * x + 0.005719;
}

float DoDiffuse(float3 N, float3 L) {
  return max(0, dot(N, L));
}

float DoSpecular(float3 V, float3 N, float3 L, float specularPower) {
  float3 R = normalize(reflect(-L, N));
  float RdotV = max(0, dot(R, V));
     
  return pow(RdotV, specularPower);
}

float DoAttenuation(float attenuation, float distance) {
  return 1.0f / (1.0f + attenuation * distance * distance);
}

float DoSpotCone(float3 spotDir, float3 L, float spotAngle) {
  float minCos = cos(spotAngle);
  float maxCos = (minCos + 1.0f) / 2.0f;
  float cosAngle = dot(spotDir, -L);
  return smoothstep(minCos, maxCos, cosAngle);
}

LightResult DoPointLight(PointLight light, float3 V, float3 P, float3 N, float specularPower) {
  LightResult result;
  float3 L = (light.PositionVS.xyz - P);
  float d = length(L);
  L = L / d;

  float attenuation = DoAttenuation(light.Attenuation, d);

  result.Diffuse = DoDiffuse(N, L) * attenuation * light.Color * light.Intensity;
  result.Specular = DoSpecular(V, N, L, specularPower) * attenuation * light.Color * light.Intensity;

  return result;
}

LightResult DoSpotLight(SpotLight light, float3 V, float3 P, float3 N, float specularPower) {
  LightResult result;
  float3 L = (light.PositionVS.xyz - P);
  float d = length(L);
  L = L / d;

  float attenuation = DoAttenuation(light.Attenuation, d);

  float spotIntensity = DoSpotCone(light.DirectionVS.xyz, L, light.SpotAngle);

  result.Diffuse = DoDiffuse(N, L) * attenuation * spotIntensity * light.Color * light.Intensity;
  result.Specular = DoSpecular(V, N, L, specularPower) * attenuation * spotIntensity * light.Color * light.Intensity;

  return result;
}

LightResult DoLighting(float3 P, float3 N, float specularPower) {
  uint i;

  // Lighting is performed in view space.
  float3 V = normalize(-P);

  LightResult totalResult = (LightResult)0;

  for (i = 0; i < LightPropertiesCB.NumPointLights; ++i) {
    LightResult result = DoPointLight(PointLights[i], V, P, N, specularPower);

    totalResult.Diffuse += result.Diffuse;
    totalResult.Specular += result.Specular;
  }

  for (i = 0; i < LightPropertiesCB.NumSpotLights; ++i) {
    LightResult result = DoSpotLight(SpotLights[i], V, P, N, specularPower);

    totalResult.Diffuse += result.Diffuse;
    totalResult.Specular += result.Specular;
  }

  return totalResult;
}

float4 main(PixelShaderInput IN) : SV_Target
{
    MaterialParameters matParams = materialParameters[IN.MatIndex];
    LightResult lit = DoLighting(IN.PositionVS.xyz, normalize(IN.NormalVS), matParams.SpecularPower);

    float4 emissive = matParams.Emissive;
    float4 ambient = matParams.Ambient;
    float4 diffuse = matParams.Diffuse * lit.Diffuse;
    float4 specular = matParams.Specular * lit.Specular;
    
    //emissive = 1;
    //diffuse = 1;
    //ambient = 1;
    //specular = lit.Specular;

    float4 texColor = DiffuseTexture[matParams.TextureIndex].Sample(LinearRepeatSampler, IN.TexCoord);

    return (emissive + ambient + diffuse + specular) * texColor;
}