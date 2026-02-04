#include "StandardVS.hlsl"

// Buffers
Texture2D tDiffuse    : register(t0);
Texture2D tNormal     : register(t1);
Texture2D tSpecular   : register(t2);
SamplerState tSampler : register(s0);

cbuffer TMaterialData : register(b0)
{
  float3 DiffuseColor;
  int Padding2;
  float3 SpecularColor;
  int Padding3;
};

cbuffer TexturesData : register(b1)
{
  int HasDiffuse;
  int HasNormal;
  int HasSpecular;
  int Padding4;
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
    float4 fDiffuseColor = float4(DiffuseColor, 1.0f);
    gBuffer.DiffuseRT = HasDiffuse ? tDiffuse.Sample(tSampler, input.uv) * fDiffuseColor : fDiffuseColor;

    // Set normal
    gBuffer.NormalRT = HasNormal ? float4(unpack_normal(tNormal.Sample(tSampler, input.uv).xyz), 1.0f) : float4(input.normal, 1.0f);

    // Set specular
    float4 fSpecularColor = float4(SpecularColor, 1.0f);
    gBuffer.SpecularRT = HasSpecular ? tSpecular.Sample(tSampler, input.uv) * fSpecularColor : fSpecularColor;
  }
  return gBuffer;
}
