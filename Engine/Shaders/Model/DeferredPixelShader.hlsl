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
  float4 color: COLOR;
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
  float3 normal = normalize(input.normal);
  float3 totalDiffuse = float3(0.0f, 0.0f, 0.0f);

  // Directional light
  float3 lightDir = normalize(directionalLight.Direction);
  float diff = max(dot(normal, lightDir), 0.0f);
  totalDiffuse += diff * directionalLight.Color * directionalLight.Intensity;
  
  // Point Lights
  for (int i = 0; i < RegisteredLights.x; i++)
  {
    PointLight pointLight = pointLights[i];
    float3 lightToPixel = pointLight.Position - input.positionWS;
    float distance = length(lightToPixel);

    if (distance < pointLight.Range)
    {
      float3 lightDirPoint = normalize(lightToPixel);
      float smoothFactor = smoothstep(pointLight.Range * 0.75f, pointLight.Range, distance);
      float attenuation = (1.0f - smoothFactor) / (1.0f + 0.14f * distance + 0.07f * (distance * distance));
      float diffPoint = max(dot(normal, lightDirPoint), 0.0f);
      totalDiffuse += diffPoint * pointLight.Color * pointLight.Intensity * attenuation;
    }
  }
  
  // Spotlights
  for (int j = 0; j < RegisteredLights.y; j++)
  {
    Spotlight spotlight = spotLights[j];
    float3 lightToPixel = spotlight.Position - input.positionWS;
    float distance = length(lightToPixel);

   if (distance < spotlight.Range)
   {
      float3 lightDirSpot = normalize(spotlight.Direction);
      float3 lightDirToPixel = normalize(lightToPixel);
      float spotFactor = dot(lightDirToPixel, lightDirSpot);
      
      if (spotFactor > cos(radians(spotlight.CutOffAngle)))
      {
        float spotAttenuation = smoothstep(cos(radians(spotlight.CutOffAngle * 0.85f)), cos(radians(spotlight.CutOffAngle)), spotFactor * -1.0f);
        float distanceAttenuation = 1.0f / (1.0f + 0.01f * distance + 0.001f * (distance * distance));
        totalDiffuse += spotlight.Color * spotlight.Intensity * spotAttenuation * distanceAttenuation;
      }
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
