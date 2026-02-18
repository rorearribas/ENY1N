#include "StandardVS.hlsl"

// Shadows vertex shader - ENY1N
cbuffer ConstantTransforms : register(b0)
{
  matrix LightViewProjection;
}

// Pixel shader input shadow
struct PS_INPUT_SHADOW
{
  float4 position : SV_POSITION;
};

PS_INPUT_SHADOW Shadows_VSMain(VS_INPUT input)
{
  PS_INPUT_SHADOW output;
  {
    float4 worldPosition = mul(input.modelMatrix, float4(input.position, 1.0));
    output.position = mul(LightViewProjection, worldPosition);
  }
  return output;
}