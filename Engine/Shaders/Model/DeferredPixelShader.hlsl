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

cbuffer LightingData : register(b0)
{
    bool HasTexture;  // Si tiene textura o no
    //DirectionalLight directionalLight; // Luz direccional
    //PointLight pointLights[50]; // Un máximo de 50 luces puntuales
    //Spotlight spotlights[50];  // Un máximo de 50 luces tipo spot
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // DirectionaLight 
    float3 normal = normalize(input.normal); // Normal del vértice o píxel
    float3 lightDir = normalize(float3(0.8f, -1.0f, 0.6f)); // Dirección de la luz (ajustada para un ángulo más estético)
    float3 lightColor = float3(1.0f, 0.9f, 0.6f); // Luz amarilla cálida
    float diff = max(dot(normal, lightDir), 0.0f); // Componente difusa
    float3 diffuse = diff * lightColor; // Combinación de intensidad difusa y color de luz

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
