// Deferred shading

// Textures
Texture2D tAlbedoTexture : register(t0);
Texture2D tRoughnessTexture : register(t1);
Texture2D tNormalTexture : register(t2);

SamplerState tAlbedoSampler : register(s0);
SamplerState tRoughnessSampler : register(s1);
SamplerState tNormalSampler : register(s2);

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : UV;
};  

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
