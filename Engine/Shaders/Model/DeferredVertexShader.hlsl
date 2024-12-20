cbuffer Camera : register(b0)
{
    matrix viewProjection;
};

cbuffer Object : register(b1)
{
    matrix modelMatrix;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color: COLOR;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color: COLOR;
    float2 uv : TEXCOORD;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    matrix worldViewProjection = mul(modelMatrix, viewProjection);
    output.position = mul(float4(input.position, 1.0), worldViewProjection);
    output.normal = input.normal;
    output.color = input.color;
    output.uv = input.uv;
    return output;
}