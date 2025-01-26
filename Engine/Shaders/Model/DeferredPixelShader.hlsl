#include "DeferredVertexShader.hlsl"

// Definir estructuras para las luces
struct DirectionalLight
{
    float3 direction; // Dirección de la luz
    float intensity;  // Intensidad de la luz
    float4 color;     // Color de la luz
};

struct PointLight
{
    float3 position; // Posición de la luz
    float intensity; // Intensidad de la luz
    float4 color;    // Color de la luz
    float attenuation; // Atenuación
};

struct Spotlight
{
    float3 position;   // Posición de la luz
    float3 direction;  // Dirección de la luz
    float cutoffAngle; // Ángulo de corte (para el cono de luz)
    float intensity;   // Intensidad de la luz
    float4 color;      // Color de la luz
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
   float3 direction; // Dirección de la luz
   float3 color;     // Color de la luz
   float intensity;  // Intensidad de la luz
}  

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // DirectionaLight 
    float3 normal = normalize(input.normal); // Pixel normal
    float diff = max(dot(normal, direction), 0.0f); // Diffuse
    float3 diffuse = diff * color * intensity; // Combination

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
