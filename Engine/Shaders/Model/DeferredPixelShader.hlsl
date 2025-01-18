#include "DeferredVertexShader.hlsl"

// Definir estructuras para las luces
struct DirectionalLight
{
    float3 direction; // Direcci�n de la luz
    float intensity;  // Intensidad de la luz
    float4 color;     // Color de la luz
};

struct PointLight
{
    float3 position; // Posici�n de la luz
    float intensity; // Intensidad de la luz
    float4 color;    // Color de la luz
    float attenuation; // Atenuaci�n
};

struct Spotlight
{
    float3 position;   // Posici�n de la luz
    float3 direction;  // Direcci�n de la luz
    float cutoffAngle; // �ngulo de corte (para el cono de luz)
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
    //PointLight pointLights[50]; // Un m�ximo de 50 luces puntuales
    //Spotlight spotlights[50];  // Un m�ximo de 50 luces tipo spot
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Si tiene textura, aplicar la textura
    if (HasTexture)
    {
       // Iluminaci�n direccional
        float3 normal = normalize(input.normal);
        float3 lightDir = normalize(float3(1.0f, 0.5, 0.));
        float diff = max(dot(normal, lightDir), 0.0f);

        float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
        return diff * texColor;
    }
    else
    {
        // Si no tiene textura, aplicar color base
        return input.color;
    }
}
