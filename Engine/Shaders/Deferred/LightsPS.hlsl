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
  // World pos
  float3 v3WorldPos = gPosition.Sample(gSampleLinear, input.uv).xyz;
  // Unpack normal
  float3 v3Normal = gNormal.Sample(gSampleLinear, input.uv).xyz /** 2.0f - 1.0f*/;
  v3Normal = normalize(v3Normal);
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
    float3 v3Diff = pointLight.Position - v3WorldPos;
    float3 v3LightDir = normalize(v3Diff);

    float fDot = dot(v3LightDir, v3Normal);
    if (fDot > 0.0f)
    {
      // Apply point light color
      float fLength = length(v3Diff);
      float fDistanceFalloff = saturate(1.0f - fLength / pointLight.Range);
      float3 v3PointDiffuse = pointLight.Color * pointLight.Intensity * fDot * fDistanceFalloff;
      v3TotalDiffuse += v3PointDiffuse;
    }
  }

  // Spot lights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 v3Diff = spotlight.Position - v3WorldPos;
    float fLength = length(v3Diff); // Distance

    // Directions
    float3 v3LightDirSpot = normalize(spotlight.Direction);
    float3 v3LightDirToPixel = normalize(v3Diff);

    float fDot = dot(v3Normal, v3LightDirToPixel);
    if (fDot > 0.0f)
    {
      // Angles
      float cosTheta = dot(-v3LightDirToPixel, v3LightDirSpot);
      float cosInner = cos(radians(15.0f));
      float cosOuter = cos(radians(35.0f));

      // Attenuation
      float fDistanceFalloff = saturate(1.0f - fLength / spotlight.Range);
      float fIntensityFalloff = saturate((cosTheta - cosOuter) / (cosInner - cosOuter));
      float fFalloff = max(fDistanceFalloff * fIntensityFalloff, 0.0f);

      // Apply spot light color
      v3TotalDiffuse += spotlight.Color * spotlight.Intensity * fFalloff;
    }
  }

  return float4(saturate(v3Diffuse * v3TotalDiffuse), 1.0f);
}
