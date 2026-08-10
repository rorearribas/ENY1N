#include "StandardVS.hlsl"
#include "global.hlsli"

cbuffer cbMaterialData : register(b0)
{
  float3 DiffuseColor;
  int HasDiffuseTexture;

  float3 SpecularColor;
  int HasSpecularTexture;

  int HasNormalTexture;
  int Padding[3];
};

struct GBuffer
{
  float4 DiffuseRT  : SV_Target0;
  float4 NormalRT   : SV_Target1;
  float4 SpecularRT : SV_Target2;
};

// Textures
Texture2D texture_diffuse    : register(t0);
Texture2D texture_normal     : register(t1);
Texture2D texture_specular   : register(t2);
SamplerState sampler_default : register(s0);

GBuffer DeferredPSMain(PS_INPUT input)
{
  GBuffer gBuffer;
  {
    // Set diffuse
    float4 fDiffuseColor = float4(DiffuseColor, 1.0f);
    gBuffer.DiffuseRT = HasDiffuseTexture ? texture_diffuse.Sample(sampler_default, input.uv) * fDiffuseColor : fDiffuseColor;

    // Set specular
    float4 fSpecularColor = float4(SpecularColor, 1.0f);
    gBuffer.SpecularRT = HasSpecularTexture ? texture_specular.Sample(sampler_default, input.uv) * fSpecularColor : fSpecularColor;

    // Set normal
		float4 fNormal = float4(input.normal, 1.0f);
		if (HasNormalTexture)
		{
      // Calculate TBN matrix
			float3x3 TBN = float3x3(input.tangent, cross(input.normal, input.tangent), input.normal);
			float3 fTextureNormal = texture_normal.Sample(sampler_default, input.uv).xyz;
			fNormal = float4(mul(unpack_normal(fTextureNormal), TBN), 1.0f);
		}
		gBuffer.NormalRT = fNormal;
	}
  return gBuffer;
}
