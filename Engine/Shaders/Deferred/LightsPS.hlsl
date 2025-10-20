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

cbuffer ConstantTransforms : register(b0)
{
  // MVP
  matrix View;
  matrix Projection;
  matrix Model;
  // Inverse
  matrix InvView;
  matrix InvProjection;
  // Projection CFG
  float FarPlane;
  float NearPlane;
  float2 Padding0;
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

float GetLinearDepth(float _fNear, float _fFar, float _zDepth)
{
  return (2.0f * _fNear) / (_fFar + _fNear - _zDepth * (_fFar - _fNear));
}

float3 GetPositionFromDepth(in float2 uv, in float z, in float4x4 InvVP)
{
  float x = uv.x * 2.0f - 1.0f;
  float y = (1.0 - uv.y) * 2.0f - 1.0f;
  float4 position_s = float4(x, y, z, 1.0f);
  float4 position_v = mul(InvVP, position_s);
  return position_v.xyz / position_v.w;
}

// GBuffer data
Texture2D gDepth    : register(t0);
Texture2D gDiffuse  : register(t1);
Texture2D gNormal   : register(t2);
Texture2D gSpecular : register(t3);
SamplerState gSampleLinear : register(s0);

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
  // Get pos + normal
  float3 v3Normal = normalize(gNormal.Sample(gSampleLinear, input.uv)).xyz;
  // Get diffuse color + specular
  float3 v3Diffuse = gDiffuse.Sample(gSampleLinear, input.uv).rgb;
  float3 v3Specular = gSpecular.Sample(gSampleLinear, input.uv).rgb;
  // Get world pos
  float fDepth = gDepth.Sample(gSampleLinear, input.uv).r;
  float3 v3WorldPos = GetPositionFromDepth(input.uv, fDepth, mul(InvView, InvProjection));

  // View direction (camera position from InvView matrix)
  float3 v3ViewDir = normalize(InvView[3].xyz - v3WorldPos);

  // Add ambient light
  float3 v3TotalLight = 0.1f * float3(1.0f, 1.0f, 1.0f);

  // Directional light
  {
    float3 v3LightDir = normalize(directionalLight.Direction);
    float fDiffuse = max(dot(v3Normal, v3LightDir), 0.0f);
    float3 v3HalfDir = normalize(v3LightDir + v3ViewDir);
    float fSpecular = pow(max(dot(v3Normal, v3HalfDir), 0.0f), 32.0f); // shininess
    // Add directional light
    v3TotalLight += directionalLight.Color * directionalLight.Intensity * (fDiffuse + fSpecular);
  }

  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float fDist = distance(pointLight.Position, v3WorldPos);
    if (fDist > pointLight.Range) 
    {
      continue;
    }

    float3 v3LightDir = normalize(pointLight.Position - v3WorldPos);
    float fDiffuse = max(dot(v3Normal, v3LightDir), 0.0f);
    float3 v3HalfDir = normalize(v3LightDir + v3ViewDir);
    float fSpecular = pow(max(dot(v3Normal, v3HalfDir), 0.0f), 32.0f);

    float fFalloff = saturate(1.0f - fDist / pointLight.Range);
    v3TotalLight += pointLight.Color * pointLight.Intensity * fFalloff * (fDiffuse + fSpecular);
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
    float fDiffuse = max(dot(v3Normal, v3LightDir), 0.0f);
    float3 v3HalfDir = normalize(v3LightDir + v3ViewDir);
    float fSpecular = pow(max(dot(v3Normal, v3HalfDir), 0.0f), 32.0f);

    float fSpotFactor = dot(-v3LightDir, v3SpotDir);
    float fInner = cos(radians(15.0f));
    float fOuter = cos(radians(35.0f));
    float fAngleFalloff = saturate((fSpotFactor - fOuter) / (fInner - fOuter));
    float fDistanceFalloff = saturate(1.0f - fDist / spotlight.Range);
    float fFalloff = fAngleFalloff * fDistanceFalloff;

    v3TotalLight += spotlight.Color * spotlight.Intensity * fFalloff * (fDiffuse + fSpecular);
  }

  // Final color
  float3 finalColor = v3Diffuse * v3TotalLight + v3Specular * v3TotalLight;
  return float4(saturate(finalColor), 1.0f);
}
