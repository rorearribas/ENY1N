// Simple vertex shader - ENY1N

cbuffer cbTransforms : register(b0)
{
  // Transforms
  matrix ViewProjection;
  matrix InvViewProjection;
};

// VS Input
struct VS_INPUT
{
  // Layout
  float3 position : VERTEXPOS;

  // Instancing
  float4x4 modelMatrix : INSTANCE_TRANSFORM;
  float3 color : COLOR;
};

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 color: COLOR;
};

PS_INPUT VSMain(VS_INPUT input)
{
  PS_INPUT output;
  {
    output.position = mul(ViewProjection, mul(input.modelMatrix, float4(input.position, 1.0)));
    output.color = input.color;
  }
  return output;
}