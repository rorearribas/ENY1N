#include "ForwardPixelShader.hlsl"

cbuffer Camera : register(b0)
{
  matrix viewProjection;
};

cbuffer ModelMatrix : register(b1)
{
  matrix modelMatrix;
};

struct VS_INPUT
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
  PS_INPUT output;
  float4 worldPosition = mul(float4(input.position, 1.0), modelMatrix);
  output.position = mul(worldPosition, viewProjection);
  output.positionWS = worldPosition.xyz;
  output.normal = input.normal;
  output.color = input.color;
  output.uv = input.uv;
  return output;
}