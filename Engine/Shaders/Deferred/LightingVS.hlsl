#include "StandardVS.hlsl"

// Lighting vertex shader - ENY1N
cbuffer cbLightingView : register(b0)
{
  matrix LightViewProjection;
}

// Pixel shader input shadow
struct PS_INPUT_LIGHTING
{
  float4 position : SV_POSITION;
};

PS_INPUT_LIGHTING Lighting_VSMain(VS_INPUT input)
{
	PS_INPUT_LIGHTING output;
  {
    float4 worldPosition = mul(input.modelMatrix, float4(input.position, 1.0));
    output.position = mul(LightViewProjection, worldPosition);
  }
  return output;
}