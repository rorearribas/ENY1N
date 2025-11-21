#include "StandardVS.hlsl"

// Buffers
Texture2D tDiffuse : register(t0);
Texture2D tNormal : register(t1);
Texture2D tSpecular : register(t2);
SamplerState tSampler: register(s0);

cbuffer TexturesData : register(b0)
{
  int HasDiffuse;
  int HasNormal;
  int HasSpecular;
  int Padding2;
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
  gOutputBuffer.gNormal = HasNormal ? float4(unpack_normal(tNormal.Sample(tSampler, input.uv).xyz), 1.0f) : float4(input.normal, 1.0f);
  // Set diffuse
  gOutputBuffer.gDiffuse = HasDiffuse ? tDiffuse.Sample(tSampler, input.uv) : float4(1.0f, 0.0f, 0.0f, 1.0f);
  // Set specular
  gOutputBuffer.gSpecular = HasSpecular ? tSpecular.Sample(tSampler, input.uv) : float4(0.0f, 0.0f, 0.0f, 0.0f);
  return gOutputBuffer;
}
