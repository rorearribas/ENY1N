#include "DrawTriangleVS.hlsl"

// PIXEL SHADER
struct DirectionalLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Direction;
  float Padding0;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Intensity;
};

struct PointLight
{
  // 12 + 4 bytes = 16 bytes
  float3 Position;
  float Padding0;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Padding1;

  // 4 + 4 bytes = 8 bytes
  float Range;
  float Intensity;
  float2 Padding;
};

struct Spotlight
{
  // 12 + 4 bytes = 16 bytes
  float3 Position;
  float Padding0;

  // 12 + 4 bytes = 16 bytes
  float3 Direction;
  float Padding1;

  // 12 + 4 bytes = 16 bytes
  float3 Color;
  float Padding2;

  // 4 + 4 + 4 + 4 = 16 bytes
  float Range;
  float Padding3;

  float Intensity;
  float Padding4;
};

// Constant buffer global lightning
cbuffer GlobalLightingData : register(b1)
{
  // Lights
  DirectionalLight directionalLight;
  PointLight pointLights[100];
  Spotlight spotLights[100];

  // Handle lights
  int2 RegisteredLights;
  float2 Padding;
};

// GBuffer data
Texture2D gPosition : register(t0);
Texture2D gDiffuse  : register(t1);
Texture2D gNormal   : register(t2);
Texture2D gSpecular : register(t3);
SamplerState gSampleLinear : register(s0);

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
  // Get pos + normal
  float3 v3WorldPos = gPosition.Sample(gSampleLinear, input.uv).xyz;
  float3 v3Normal = normalize(gNormal.Sample(gSampleLinear, input.uv)).xyz;
  // Get diffuse color + specular
  float3 v3Diffuse = gDiffuse.Sample(gSampleLinear, input.uv).rgb;
  float3 v3Specular = gSpecular.Sample(gSampleLinear, input.uv).rgb;

  // Ambient light
  float3 v3TotalDiffuse = float3(0.0f, 0.0f, 0.0f);
  float3 v3AmbientColor = 0.01f * float3(1.0f, 1.0f, 1.0f);
  v3TotalDiffuse += v3AmbientColor;

  // Directional light
  float3 v3LightDir = normalize(directionalLight.Direction);
  float fDot = max(dot(v3Normal, v3LightDir), 0.0f);
  v3TotalDiffuse += directionalLight.Color * directionalLight.Intensity * fDot;

  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float fDist = distance(pointLight.Position, v3WorldPos);
    if (fDist > pointLight.Range)
    {
      continue;
    }

    // Get dir
    float3 v3LightDir = normalize(pointLight.Position - v3WorldPos);
    float fDot = dot(v3LightDir, v3Normal);
    if (fDot > 0.0f)
    {
      // Calculate falloff
      float fDistanceFalloff = saturate(1.0f - fDist / pointLight.Range);
      float3 v3PointDiffuse = pointLight.Color * pointLight.Intensity * fDot * fDistanceFalloff;
      // Apply color
      v3TotalDiffuse += v3PointDiffuse;
    }
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

    float3 v3LightDir = normalize(spotlight.Position - v3WorldPos);
    float3 v3SpotDir = normalize(spotlight.Direction);
    float fDot = max(dot(v3Normal, v3LightDir), 0.0f);
    if (fDot > 0.0f)
    {
      // Calculate cone
      float fSpotFactor = dot(-v3LightDir, v3SpotDir);
      float fInner = cos(radians(15.0f));
      float fOuter = cos(radians(35.0f));
      // Calculate falloff
      float fAngleFalloff = saturate((fSpotFactor - fOuter) / (fInner - fOuter));
      float fDistanceFalloff = saturate(1.0f - fDist / spotlight.Range);
      float fFalloff = fAngleFalloff * fDistanceFalloff;
      // Apply color
      v3TotalDiffuse += spotlight.Color * spotlight.Intensity * fDot * fFalloff;
    }
  }
  return float4(saturate(v3Diffuse * v3TotalDiffuse), 1.0f);
}
