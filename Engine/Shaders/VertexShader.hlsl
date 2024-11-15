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
    float4 color : COLOR;
};

struct PS_INPUT 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input) 
{
    PS_INPUT output;
    matrix world = mul(modelMatrix, viewProjection);
    output.position = mul(float4(input.position, 1.0), world);
    output.color = input.color;
    return output;
}