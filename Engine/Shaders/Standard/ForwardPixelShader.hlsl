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
  float Intensity;
  float Range;
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
  float CutOffAngle;
  float Range;
  float Intensity;
  float Padding3;
};

// Samplers
Texture2D cTexture2D : register(t0);
SamplerState cSamplerState : register(s0);

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 positionWS : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : TEXCOORD;
};

// Constant buffer
cbuffer ConstantTexture : register(b0)
{
  bool HasTexture;
  float3 Padding0;
}

// Constant buffer global lightning
cbuffer GlobalLightningData : register(b1)
{
  // Lights
  DirectionalLight directionalLight;
  PointLight pointLights[100];
  Spotlight spotLights[100];
  // Handle lights
  int2 RegisteredLights;
  float2 Padding;
}  

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  float3 vNormal = normalize(input.normal);
  float3 vTotalDiffuse = float3(0.0f, 0.0f, 0.0f);

  // Directional light
  float3 vLightDir = normalize(directionalLight.Direction);
  float vDiff = max(dot(vNormal, vLightDir), 0.0f);
  vTotalDiffuse += vDiff * directionalLight.Color * directionalLight.Intensity;
  
  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float3 vlightToPixel = pointLight.Position - input.positionWS;
    float fDistance = length(vlightToPixel);

    if (fDistance < pointLight.Range)
    {
      float3 vLightDirPoint = normalize(vlightToPixel);
      float fSmoothFactor = smoothstep(pointLight.Range * 0.75f, pointLight.Range, fDistance);
      float fAttenuation = (1.0f - fSmoothFactor) / (1.0f + 0.14f * fDistance + 0.07f * (fDistance * fDistance));
      float fDiffPoint = max(dot(vNormal, vLightDirPoint), 0.0f);
      vTotalDiffuse += fDiffPoint * pointLight.Color * pointLight.Intensity * fAttenuation;
    }
  }
  
  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 vLightToPixel = spotlight.Position - input.positionWS;
    float fDistance = length(vLightToPixel);

   if (fDistance < spotlight.Range)
   {
      float3 vLightDirSpot = normalize(spotlight.Direction);
      float3 vLightDirToPixel = normalize(vLightToPixel);
      float fSpotFactor = dot(vLightDirToPixel, vLightDirSpot);
      
      if (fSpotFactor > cos(radians(spotlight.CutOffAngle)))
      {
        float fSpotAttenuation = smoothstep(cos(radians(spotlight.CutOffAngle * 0.85f)), cos(radians(spotlight.CutOffAngle)), fSpotFactor * -1.0f);
        float fDistanceAttenuation = 1.0f / (1.0f + 0.01f * fDistance + 0.001f * (fDistance * fDistance));
        vTotalDiffuse += spotlight.Color * spotlight.Intensity * fSpotAttenuation * fDistanceAttenuation;
      }
    }
  }

  if (HasTexture)
  {
    float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
    return float4(vTotalDiffuse, 1.0f) * texColor;
  }
  
  return float4(vTotalDiffuse, 1.0f) * float4(input.color, 1.0f);
}
