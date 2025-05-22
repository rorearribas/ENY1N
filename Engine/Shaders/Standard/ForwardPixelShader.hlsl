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
  float CutOffAngle;
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
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : TEXCOORD;
};

// Constant buffer
cbuffer ConstantTexture : register(b0)
{
  bool HasTexture;
  float Padding0;

  bool HasModel;
  float Padding1;
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
  float4 v4TextureColor = cTexture2D.Sample(cSamplerState, input.uv);
  float3 v3TotalDiffuse = float3(0.0f, 0.0f, 0.0f);
  float3 v3Normal = normalize(input.normal);

  // Ambient light
  float3 v3AmbientColor = 0.02f * float3(1.0f, 1.0f, 1.0f);
  v3TotalDiffuse += v3AmbientColor;

  if(!HasModel)
  {
    float3 dp1 = ddx(input.worldpos);
    float3 dp2 = ddy(input.worldpos);
    v3Normal = normalize(cross(dp1, dp2));
  }

  // Directional light
  if(directionalLight.Intensity > 0.001f)
  {
    float3 v3LightDir = normalize(directionalLight.Direction);
    float fDot = max(dot(v3Normal, v3LightDir), 0.0f);
    v3TotalDiffuse += fDot * directionalLight.Color * directionalLight.Intensity;
  }
  
  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float3 v3Diff = pointLight.Position - input.worldpos;
    float fDistance = length(v3Diff);

    float3 v3LightDir = normalize(v3Diff);
    float fDot = max(dot(v3Normal, v3LightDir), 0.0f);
    if(fDot > 0.0f)
    {
      float fAttenuation = 1.0f / (1.0f + 0.001f * fDistance + 0.001f * fDistance * fDistance);
      float fIntensityFalloff = saturate(1.0f - pow(fDistance / pointLight.Range, 0.25f));
      float3 vPointDiffuse = fDot * pointLight.Color * pointLight.Intensity * fAttenuation * fIntensityFalloff;
      v3TotalDiffuse += vPointDiffuse;
    }
  }
  
  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 v3Diff = spotlight.Position - input.worldpos;
    float fDistance = length(v3Diff); // Distance

    if(fDistance > spotlight.Range)
    {
      continue;
    }

    // Directions
    float3 v3LightDirSpot = normalize(spotlight.Direction);
    float3 v3LightDirToPixel = normalize(v3Diff);

    // Dots
    float fSpotDot = dot(v3LightDirToPixel, v3LightDirSpot);
    float fDiffuseDot = max(dot(v3Normal, v3LightDirToPixel), 0.0f);
    
    if (fSpotDot > cos(radians(spotlight.CutOffAngle)))
    {
      float fSpotAttenuation = smoothstep(cos(radians(spotlight.CutOffAngle * 0.85f)), cos(radians(spotlight.CutOffAngle)), fSpotDot);
      float fDistanceAttenuation = 1.0f / (1.0f + 0.045f * fDistance + 0.0075f * fDistance * fDistance);
      v3TotalDiffuse += spotlight.Color * spotlight.Intensity * fSpotAttenuation * fDistanceAttenuation * fDiffuseDot;
    }
  }
  
  return float4(v3TotalDiffuse, 1.0f) * (HasTexture ? v4TextureColor : float4(input.color, 1.0f));
}
