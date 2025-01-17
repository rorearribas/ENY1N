#include "DeferredVertexShader.hlsl"

Texture2D cTexture2D : register(t0);
SamplerState cSamplerState : register(s0);

cbuffer Texture : register(b0)
{
  bool HasTexture = false;
}

float4 PSMain(PS_INPUT input) : SV_TARGET 
{
  // Direction light
  float3 lightDir = normalize(float3(0.5f, 1.0f, 1.0f));
  float3 normal = normalize(input.normal);
  
  // Diffuse illumination
  float diff = max(dot(normal, lightDir), 0.0f);

  if(HasTexture)
  {
    float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
    return texColor;
  }
  else
  {
    //Color base
    return diff * input.color;
  }
}