#include "SimpleVS.hlsl"

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

  // 4 + 4 + 8 bytes = 16 bytes
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

  // 4 + 4 + 8 = 16 bytes
  float Range;
  float Intensity;
  float2 Padding3;
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
  float2 Padding1;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  float3 v3Color = input.color;
  float3 v3Normal = input.normal;
  float3 v3WorldPos = input.worldpos;

  // Add ambient light
  float3 v3TotalLight = 0.1f * float3(1.0f, 1.0f, 1.0f);

  // Directional light
  float3 v3LightDir = normalize(directionalLight.Direction);
  float fDot = max(dot(v3Normal, -v3LightDir), 0.0f);
  v3TotalLight += directionalLight.Color * directionalLight.Intensity * fDot;

  // Point lights
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
    float fFalloff = saturate(1.0f - fDist / pointLight.Range);
    fDot = max(dot(v3Normal, v3LightDir), 0.0f);

    // Add light
    v3TotalLight += pointLight.Color * pointLight.Intensity * fDot * fFalloff;
  }

  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotLight = spotLights[j];
    float fDist = distance(spotLight.Position, v3WorldPos);
    if (fDist > spotLight.Range)
    {
      continue;
    }

    // Calculate spot
    float3 v3LightDir = normalize(spotLight.Position - v3WorldPos);
    float3 v3SpotDir = normalize(spotLight.Direction);
    fDot = max(dot(v3Normal, v3LightDir), 0.0f);

    float fSpotFactor = dot(-v3LightDir, v3SpotDir);
    float fInner = cos(radians(15.0f));
    float fOuter = cos(radians(35.0f));
    float fAngleFalloff = saturate((fSpotFactor - fOuter) / (fInner - fOuter));
    float fDistFalloff = saturate(1.0f - fDist / spotLight.Range);

    // Add light
    v3TotalLight += spotLight.Color * spotLight.Intensity * fAngleFalloff * fDistFalloff * fDot;
  }
  return float4(saturate(v3TotalLight * v3Color), 1.0f);
}
