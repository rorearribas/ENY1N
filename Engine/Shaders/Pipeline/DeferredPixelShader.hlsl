// Diffuse
Texture2D tDiffuse : register(t0);
SamplerState tDiffuseSampler : register(s0);

// Normal
Texture2D tNormal : register(t1);
SamplerState tNormalSampler : register(s1);

// Specular
Texture2D tSpecular : register(t2);
SamplerState tSpecularSampler: register(s2);

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : UV;
};

struct GBuffer
{
  float4 gPosition : SV_Target0;
  float4 gDiffuse : SV_Target1;
  float4 gNormal : SV_Target2;
  float4 gSpecular : SV_Target3;
};

GBuffer DeferredPSMain(PS_INPUT input)
{
  GBuffer gBuffer;

  gBuffer.gPosition = input.position;
  gBuffer.gDiffuse = tDiffuse.Sample(tDiffuseSampler, input.uv);
  gBuffer.gNormal = tNormal.Sample(tNormalSampler, input.uv);
  gBuffer.gSpecular = tSpecular.Sample(tSpecularSampler, input.uv);

  return gBuffer;
}
