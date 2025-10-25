// Simple vertex shader

cbuffer ConstantTransforms : register(b0)
{
  // MVP
  matrix View;
  matrix Projection;
  matrix Model;
  // Inverse
  matrix InvView;
  matrix InvProjection;
  // Projection CFG
  float FarPlane;
  float NearPlane;
  float2 Padding0;
};

// Vertex input
struct VS_INPUT
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : UV;
};

// Pixel input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : UV;
};

PS_INPUT VSMain(VS_INPUT input)
{
  PS_INPUT output;

  // MVP
  float4 worldPosition = mul(Model, float4(input.position, 1.0));
  matrix viewProjection = mul(Projection, View);
  output.position = mul(viewProjection, worldPosition);
  output.worldpos = worldPosition.xyz;

  float3x3 normalMatrix = transpose((float3x3)Model);
  output.normal = normalize(mul(Model, input.normal));

  output.color = input.color;
  output.uv = input.uv;

  return output;
}