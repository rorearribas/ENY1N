// Standard vertex shader - ENY1N

cbuffer ConstantTransforms : register(b0)
{
  // Transforms
  matrix ViewProjection;
  matrix InvViewProjection;

  // Projection CFG
  float FarPlane;
  float NearPlane;
  float2 Padding0;
};

// VS Input
struct VS_INPUT
{
  // Vertex info
  float3 position : VERTEXPOS;
  float3 normal : NORMAL;
  float2 uv : UV;

  // Instancing
  float4x4 modelMatrix : INSTANCE_TRANSFORM;
};

// PS Input
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float2 uv : UV;
};

PS_INPUT VSMain(VS_INPUT input)
{
  PS_INPUT output;
  {
    float4 worldPosition = mul(input.modelMatrix, float4(input.position, 1.0));
    output.position = mul(ViewProjection, worldPosition);
    output.worldpos = worldPosition.xyz;

    float3x3 normalMatrix = transpose((float3x3)input.modelMatrix);
    output.normal = normalize(mul(normalMatrix, input.normal));
    output.uv = input.uv;
  }
  return output;
}