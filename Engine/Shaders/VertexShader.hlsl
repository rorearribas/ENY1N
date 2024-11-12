cbuffer ConstantBuffer : register(b0) 
{
   float4x4 mat;
};

struct VS_INPUT {
    float3 position : POSITION;
    //float3 normal : NORMAL;
    //float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    //float3 normal : NORMAL;
    //float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input) 
{
    PS_INPUT output;
    // Aplica las transformaciones de mundo, vista y proyección
    output.position = mul(float4(input.position, 1.0f), mat);
    // Pasa el color al shader de píxeles
    output.color = input.color;
    return output;
}