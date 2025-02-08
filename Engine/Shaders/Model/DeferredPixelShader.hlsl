#include "DeferredVertexShader.hlsl"

// Definir estructuras para las luces
struct DirectionalLight
{
    float3 Direction; // Dirección de la luz
    float Padding1; // Padding
    float3 Color; // Color de la luz
    float Intensity;  // Intensidad de la luz
};

struct PointLight
{
    float3 position; // Posición de la luz
    float4 color;    // Color de la luz
    float intensity; // Intensidad de la luz
    float attenuation; // Atenuación
};

struct Spotlight
{
    float3 Position;   // Posición de la luz
    float3 Direction;  // Dirección de la luz
    float4 Color;      // Color de la luz
    float Intensity;   // Intensidad de la luz
    float CutOffAngle; // Ángulo de corte (para el cono de luz)
};

// Samplers y buffers
Texture2D cTexture2D : register(t0);
SamplerState cSamplerState : register(s0);

cbuffer ConstantTexture : register(b0)
{
    bool HasTexture;
}

cbuffer LightningData : register(b1)
{
   DirectionalLight directionalLight;
}  

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // DirectionaLight 
    float3 normal = normalize(input.normal); // Pixel normal
    float diff = max(dot(normal, directionalLight.Direction), 0.0f); // Diffuse
    float3 diffuse = diff * directionalLight.Color * directionalLight.Intensity; // Combination

    // Apply texture color + illu
    if (HasTexture)
    {
        float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
        return float4(diffuse, 1.0f) * texColor;
    }
    else
    {
        return float4(diffuse, 1.0f) * input.color;
    }
}
