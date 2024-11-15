//Texture2D myTexture : register(t0);
//SamplerState mySampler : register(s0);

struct PS_INPUT 
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 PSMain(PS_INPUT input) : SV_TARGET 
{
    // Luz direccional
    //float3 lightDir = normalize(float3(0.5f, -1.0f, -0.5f));
    //float3 normal = normalize(input.normal);

    // Cálculo de iluminación difusa (Lambert)
    //float diff = max(dot(normal, lightDir), 0.0f);

    // Color base
    //float4 texColor = myTexture.Sample(mySampler, input.texcoord);
    //float4 finalColor = texColor * input.color * diff;

    return input.color;
}