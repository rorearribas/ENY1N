#include "DeferredVertexShader.hlsl"

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
  float3 normal = normalize(input.normal);
  float3 totalDiffuse = float3(0.0f, 0.0f, 0.0f);

  // Dirección de la luz direccional
  float3 lightDir = normalize(directionalLight.Direction);
  float diff = max(dot(normal, lightDir), 0.0f);
  totalDiffuse += diff * directionalLight.Color * directionalLight.Intensity;
  
  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    
    float3 lightToPixel = pointLight.Position - input.position; // Corrección de dirección
    float distance = length(lightToPixel);
    float3 lightDirPoint = normalize(lightToPixel);
    
    float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * (distance * distance));
    float diffPoint = max(dot(normal, lightDirPoint), 0.0f);
    
    totalDiffuse += diffPoint * pointLight.Color * pointLight.Intensity * attenuation;
  }
  
  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 lightToPixel = normalize(input.position - spotlight.Position); // Dirección corregida
    float3 lightDirSpot = normalize(spotlight.Direction);
    
    float spotFactor = dot(-lightToPixel, lightDirSpot); // Factor corregido
    if (spotFactor > cos(radians(spotlight.CutOffAngle))) // Conversión a radianes
    {
      float distance = length(input.position - spotlight.Position);
      float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * (distance * distance));
      totalDiffuse += max(spotFactor, 0.0f) * spotlight.Color * spotlight.Intensity * attenuation;
    }
  }

  if (HasTexture)
  {
    float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
    return float4(totalDiffuse, 1.0f) * texColor;
  }
  else
  {
    return float4(totalDiffuse, 1.0f) * input.color;
  }
}
