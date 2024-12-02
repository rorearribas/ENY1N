#include "MVertexShader.hlsl"

Texture2D cTexture2D : register(t0);
SamplerState cSamplerState : register(s0);

float4 PSMain(PS_INPUT input) : SV_TARGET 
{
    // Luz direccional
    float3 lightDir = normalize(float3(0.5f, 1.0f, -1.0f));
    float3 normal = normalize(input.normal);

    // Cálculo de iluminación difusa (Lambert)
    float diff = max(dot(normal, lightDir), 0.0f);

    //Color base
    float4 texColor = cTexture2D.Sample(cSamplerState, input.uv);
    float4 finalColor = texColor * diff * input.color;

    return finalColor;
}