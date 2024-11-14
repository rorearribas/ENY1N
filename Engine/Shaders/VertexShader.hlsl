cbuffer ConstantBuffer : register(b0) 
{
   matrix mat;
};

struct VS_INPUT {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT VSMain(VS_INPUT input) 
{
    PS_INPUT output;
    output.position = mul(float4(input.position, 1.0f), mat);
    output.color = input.color;
    return output;
}