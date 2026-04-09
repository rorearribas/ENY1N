#include "DrawTriangleVS.hlsl"

// PIXEL SHADER
struct DirectionalLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Direction;
  float Intensity;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  int CastShadows;
};

struct PointLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Position;
  float Range;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Intensity;
};

struct Spotlight
{
  // 12 + 4 bytes = 16 bytes
  float3 Position;
  float Padding0;

  // 12 + 4 bytes = 16 bytes
  float3 Direction;
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

float get_linear_depth(float near, float far, float depth)
{
  return (2.0f * near) / (far + near - depth * (far - near));
}

float3 get_pos_from_depth(in float2 uv, in float z, in float4x4 InvVP)
{
  float x = uv.x * 2.0f - 1.0f;
  float y = (1.0 - uv.y) * 2.0f - 1.0f;
  float4 position_v = mul(InvVP, float4(x, y, z, 1.0f));
  return position_v.xyz / position_v.w;
}

float2 texture_size(Texture2D tex)
{
  uint width, height;
  tex.GetDimensions(width, height);
  return float2(width, height);
}

float2 texel_scale(Texture2D tex)
{
  return 1.0f / texture_size(tex);
}

float3 offset_lookup(Texture2D tex, SamplerComparisonState sampl, float2 uv, float2 texel_size, float2 offset, float current_depth)
{
  return tex.SampleCmpLevelZero(sampl, uv + offset * texel_size, current_depth);
}

float2 get_uvs_from_light_space(float4 posLightSpace)
{
  float3 proj_coords = posLightSpace.xyz / posLightSpace.w;
  float2 shadow_uv = proj_coords.xy * 0.5f + 0.5f;
  return float2(shadow_uv.x, 1.0f - shadow_uv.y);
}

float ComputeShadowMapping(Texture2D tex, SamplerComparisonState sampl, float2 shadows_uv, float current_depth, uint samples)
{
  float2 texelScale = texel_scale(tex);

  float fSum = 0.0f;
  for (float y = -1.5; y <= 1.5; y += 1.0)
  {
    for (float x = -1.5; x <= 1.5; x += 1.0)
    {
      fSum += offset_lookup(tex, sampl, shadows_uv, texelScale, float2(x, y), current_depth);
    }
  }
  return fSum / samples;
}

// GBuffer data
Texture2D texture_depth      : register(t0);
Texture2D texture_diffuse    : register(t1);
Texture2D texture_normal     : register(t2);
Texture2D texture_specular   : register(t3);

// Shadow map - Test
Texture2D texture_shadowmap  : register(t4);

// Sampler
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

    fShadowFactor = ComputeShadowMapping
    (
      texture_shadowmap, 
      sampler_shadows, 
      get_uvs_from_light_space(posLightSpace), 
      current_shadow_depth,
      max_samples
    );
  }

  float3 v3LightDir = normalize(directionalLight.Direction);
  float fDot = max(dot(v3Normal, -v3LightDir), 0.0f);
  v3TotalLight += (directionalLight.Color * directionalLight.Intensity * fDot) * fShadowFactor;

  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float fDist = distance(pointLight.Position, v3WorldPos);
    if (fDist > pointLight.Range)
    {
      continue;
    }

    // Calculate point light
    float3 v3LightDir = normalize(pointLight.Position - v3WorldPos);
    float fDiffuse = max(dot(v3Normal, v3LightDir), 0.0f);
    float fFalloff = saturate(1.0f - fDist / pointLight.Range);

    // Add light
    v3TotalLight += pointLight.Color * pointLight.Intensity * fFalloff;
  }

  // Spot lights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float fDist = distance(spotlight.Position, v3WorldPos);
    if (fDist > spotlight.Range)
    {
      continue;
    }

    // Calculate spot light
    float3 v3LightDir = normalize(spotlight.Position - v3WorldPos);
    float3 v3SpotDir = normalize(spotlight.Direction);

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
