// Simple vertex shader

cbuffer Camera : register(b0)
{
  matrix viewProjection;
};

cbuffer ModelMatrix : register(b1)
{
  matrix modelMatrix;
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

  float4 worldPosition = mul(modelMatrix, float4(input.position, 1.0));
  output.position = mul(viewProjection, worldPosition);
  output.worldpos = worldPosition.xyz;
  float3x3 normalMatrix = transpose((float3x3)modelMatrix);
  output.normal = normalize(mul(normalMatrix, input.normal));
  output.color = input.color;
  output.uv = input.uv;

  return output;
}