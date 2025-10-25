#include "SimpleVS.hlsl"

// Diffuse
Texture2D tDiffuse : register(t0);
SamplerState tDiffuseSampler : register(s0);

// Normal
Texture2D tNormal : register(t1);
SamplerState tNormalSampler : register(s1);

// Specular
Texture2D tSpecular : register(t2);
SamplerState tSpecularSampler: register(s2);

cbuffer TexturesData : register(b0)
{
  int HasDiffuse;
  int HasNormal;
  int HasSpecular;
  int Padding1;
};

struct GBuffer
{
  float4 gDiffuse : SV_Target0;
  float4 gNormal : SV_Target1;
  float4 gSpecular : SV_Target2;
};

float3 unpack_normal(float3 _v3Normal)
{
  return _v3Normal * 2.0f - 1.0f;
}

GBuffer DeferredPSMain(PS_INPUT input)
{
  GBuffer gOutputBuffer;
  // Set normal
  gOutputBuffer.gNormal = /*HasNormal ? float4(unpack_normal(tNormal.Sample(tNormalSampler, input.uv).xyz), 1.0f) :*/ float4(input.normal, 1.0f);
  // Set diffuse
  gOutputBuffer.gDiffuse = HasDiffuse ? tDiffuse.Sample(tDiffuseSampler, input.uv) : float4(input.color, 1.0f);
  // Set specular
  gOutputBuffer.gSpecular = HasSpecular ? tSpecular.Sample(tSpecularSampler, input.uv) : float4(0.0f, 0.0f, 0.0f, 0.0f);
  return gOutputBuffer;
}
