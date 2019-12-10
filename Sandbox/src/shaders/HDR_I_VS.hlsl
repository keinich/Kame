struct Mat {
  matrix ModelMatrix;
  matrix ModelViewMatrix;
  matrix InverseTransposeModelViewMatrix;
  matrix ModelViewProjectionMatrix;
};

struct InstanceData {
  matrix ModelMatrix;
  matrix ModelViewMatrix;
  matrix InverseTransposeModelViewMatrix;
  matrix ModelViewProjectionMatrix;
};

StructuredBuffer<InstanceData> gInstanceData : register(t0, space1);
ConstantBuffer<Mat> MatCB : register(b0);


struct VertexPositionNormalTexture {
  float3 Position : POSITION;
  float3 Normal   : NORMAL;
  float2 TexCoord : TEXCOORD;
};

struct VertexShaderOutput {
  float4 PositionVS : POSITION;
  float3 NormalVS   : NORMAL;
  float2 TexCoord   : TEXCOORD;
  float4 Position   : SV_Position;
};

VertexShaderOutput main(VertexPositionNormalTexture IN, uint instanceID : SV_InstanceID) {
  VertexShaderOutput OUT;

  //OUT.Position = mul(MatCB.ModelViewProjectionMatrix, float4(IN.Position, 1.0f));
  //matrix vp = mul(MatCB.)
  OUT.Position = mul(gInstanceData[instanceID].ModelViewProjectionMatrix, float4(IN.Position, 1.0f));
  OUT.PositionVS = mul(gInstanceData[instanceID].ModelViewMatrix, float4(IN.Position, 1.0f));
  OUT.NormalVS = mul((float3x3)gInstanceData[instanceID].InverseTransposeModelViewMatrix, IN.Normal);
  OUT.TexCoord = IN.TexCoord;

  return OUT;
}