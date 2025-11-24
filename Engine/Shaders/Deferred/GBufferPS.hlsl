#include "StandardVS.hlsl"

// Buffers
Texture2D tDiffuse    : register(t0);
Texture2D tNormal     : register(t1);
Texture2D tSpecular   : register(t2);
SamplerState tSampler : register(s0);

cbuffer TexturesData : register(b0)
{
  int HasDiffuse;
  int HasNormal;
  int HasSpecular;
  int Padding2;
};

struct GBuffer
{
  float4 DiffuseRT  : SV_Target0;
  float4 NormalRT   : SV_Target1;
  float4 SpecularRT : SV_Target2;
};

float3 unpack_normal(float3 _v3Normal)
{
  return _v3Normal * 2.0f - 1.0f;
}

GBuffer DeferredPSMain(PS_INPUT input)
{
  GBuffer gBuffer;
  {
    // Set diffuse
    gBuffer.DiffuseRT = HasDiffuse ? tDiffuse.Sample(tSampler, input.uv) : float4(1.0f, 0.0f, 0.0f, 1.0f);
    // Set normal
    gBuffer.NormalRT = HasNormal ? float4(unpack_normal(tNormal.Sample(tSampler, input.uv).xyz), 1.0f) : float4(input.normal, 1.0f);
    // Set specular
    gBuffer.SpecularRT = HasSpecular ? tSpecular.Sample(tSampler, input.uv) : float4(0.0f, 0.0f, 0.0f, 0.0f);
  }
  return gBuffer;
}
