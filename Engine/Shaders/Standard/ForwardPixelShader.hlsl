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

// Samplers
Texture2D cTexture2D : register(t0);
SamplerState cSamplerState : register(s0);

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : TEXCOORD;
};

// Constant buffer
cbuffer ConstantTexture : register(b0)
{
  // 4 + 4 + 4
  int HasTexture = 0;
  int HasModel = 0;
  int UseGlobalLighting = 0;

  // 12 + 4
  int Padding0;
};

// Constant buffer global lightning
cbuffer GlobalLightingData : register(b1)
{
  // Lights
  DirectionalLight directionalLight;
  PointLight pointLights[250];
  Spotlight spotLights[250];

  // Handle lights
  int2 RegisteredLights;
  float2 Padding;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  float4 v4TextureColor = cTexture2D.Sample(cSamplerState, input.uv);
  float3 v3TotalDiffuse = float3(0.0f, 0.0f, 0.0f);
  float3 v3Normal = normalize(input.normal);

  // Ambient light
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
    float3 v3Diff = pointLight.Position - input.worldpos;
    float fLength = length(v3Diff);

    float3 v3LightDir = normalize(v3Diff);
    float fDot = max(dot(v3Normal, v3LightDir), 0.0f);
    if(fDot > 0.0f)
    {
      // Apply point light color
      float fDistanceFalloff = saturate(1.0f - fLength / pointLight.Range);
      float3 v3PointDiffuse = pointLight.Color * pointLight.Intensity * fDot * fDistanceFalloff;
      v3TotalDiffuse += v3PointDiffuse;
    }
  }
  
  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 v3Diff = spotlight.Position - input.worldpos;
    float fLength = length(v3Diff); // Distance

    // Directions
    float3 v3LightDirSpot = normalize(spotlight.Direction);
    float3 v3LightDirToPixel = normalize(v3Diff);

    float fDot = dot(v3Normal, v3LightDirToPixel);
    if(fDot > 0.0f)
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

  if(UseGlobalLighting)
  {
    float4 v4FinalColor = HasTexture ? v4TextureColor : float4(input.color, 1.0f);
    return float4(saturate(v4FinalColor.rgb * 0.5 + v3TotalDiffuse * 0.5f), 1.0f);
  }

  return float4(input.color, 1.0f);
}
