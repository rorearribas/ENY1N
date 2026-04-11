#include "DrawTriangleVS.hlsl"
#include "global.hlsli"

// Pixel Shader
struct DirectionalLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Dir;
  float Intensity;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  int CastShadows;
};

struct PointLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Pos;
  float Range;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Intensity;
};

struct Spotlight
{
  // 12 + 4 bytes = 16 bytes
  float3 Pos;
  float Padding0;

  // 12 + 4 bytes = 16 bytes
  float3 Dir;
  float Range;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Intensity;
};

// Transforms
cbuffer ConstantTransforms : register(b0)
{
  // Transforms
  matrix ViewProjection;
  matrix InvViewProjection;
  matrix LightViewProjection;

  // Projection CFG
  float FarPlane;
  float NearPlane;
  float2 Padding0;
};

// Lighting
cbuffer CLightingData : register(b1)
{
  // Lights
  DirectionalLight directionalLight;
  PointLight pointLights[100];
  Spotlight spotLights[100];

  // Handle lights
  int2 RegisteredLights;
  float2 Padding1;
};

// GBuffer
Texture2D texture_depth    : register(t0);
Texture2D texture_diffuse  : register(t1);
Texture2D texture_normal   : register(t2);
Texture2D texture_specular : register(t3);

// Shadow mapping
Texture2D texture_shadowmap : register(t4);

// Samplers
SamplerState sampler_default : register(s0);
SamplerComparisonState sampler_shadows : register(s1);

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
  // Get pos + normal
  float3 v3Normal = normalize(texture_normal.Sample(sampler_default, input.uv)).xyz;

  // Get diffuse color + specular
  float3 v3Diffuse = texture_diffuse.Sample(sampler_default, input.uv).rgb;
  float3 v3Specular = texture_specular.Sample(sampler_default, input.uv).rgb;

  // Get world pos
  float fDepth = texture_depth.Sample(sampler_default, input.uv).r;
  float3 v3WorldPos = get_pos_from_depth(input.uv, fDepth, InvViewProjection);

  // Add ambient light
  float3 v3TotalLight = float3(1.0f, 1.0f, 1.0f) * 0.2f;

  float fShadowFactor = 1.0f;
  if (directionalLight.CastShadows)
  {
    // Calculate shadows
    const uint max_samples = 16;
    float4 posLightSpace = mul(LightViewProjection, float4(v3WorldPos, 1.0f));
    float current_shadow_depth = float3(posLightSpace.xyz / posLightSpace.w).z;

    fShadowFactor = compute_shadow_mapping
    (
      texture_shadowmap, 
      sampler_shadows, 
      get_uvs_from_light_space(posLightSpace), 
      current_shadow_depth,
      max_samples
    );
  }

  float fDiffuseFactor = saturate(dot(directionalLight.Dir, v3Normal));
  v3TotalLight += (fDiffuseFactor * directionalLight.Color * directionalLight.Intensity) * fShadowFactor;

  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float fDist = distance(pointLight.Pos, v3WorldPos);
    if (fDist > pointLight.Range)
    {
      continue;
    }

    // Calculate point light
    float3 v3LightDir = normalize(pointLight.Pos - v3WorldPos);
    float fDiffuse = saturate(dot(v3LightDir, v3Normal));
    float fFalloff = saturate(1.0f - fDist / pointLight.Range);

    // Add light
    v3TotalLight += pointLight.Color * pointLight.Intensity * fFalloff;
  }

  // Spot lights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float fDist = distance(spotlight.Pos, v3WorldPos);
    if (fDist > spotlight.Range)
    {
      continue;
    }

    // Calculate spot light
    float3 v3LightDir = normalize(spotlight.Pos - v3WorldPos);
    float3 v3SpotDir = normalize(spotlight.Dir);

    float fSpotFactor = dot(-v3LightDir, v3SpotDir);
    float fInner = cos(radians(15.0f));
    float fOuter = cos(radians(35.0f));
    float fAngleFalloff = saturate((fSpotFactor - fOuter) / (fInner - fOuter));
    float fDistanceFalloff = saturate(1.0f - fDist / spotlight.Range);
    float fFalloff = fAngleFalloff * fDistanceFalloff;

    // Add color
    v3TotalLight += spotlight.Color * spotlight.Intensity * fFalloff;
  }

  return float4(saturate(v3TotalLight * v3Diffuse), 1.0f);
}
