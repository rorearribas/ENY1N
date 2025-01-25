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
        float3 normal = normalize(input.normal); // Normal del v�rtice o p�xel
        float3 lightDir = normalize(float3(0.8f, -1.0f, 0.6f)); // Direcci�n de la luz (ajustada para un �ngulo m�s est�tico)
        float3 lightColor = float3(1.0f, 0.9f, 0.6f); // Luz amarilla c�lida
        float diff = max(dot(normal, lightDir), 0.0f); // Componente difusa
        float3 diffuse = diff * lightColor; // Combinaci�n de intensidad difusa y color de luz
        float4 finalcolor = float4(diffuse, 1.0f);

        float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
        return finalcolor * texColor;
    }
    else
    {
        // Si no tiene textura, aplicar color base
        return input.color;
    }
}
