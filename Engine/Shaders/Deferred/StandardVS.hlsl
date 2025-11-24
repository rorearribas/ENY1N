// Standard vertex shader - ENY1N

cbuffer ConstantTransforms : register(b0)
{
  // Transforms
  matrix Model;
  matrix ViewProjection;
  matrix InvViewProjection;

  // Projection CFG
  float FarPlane;
  float NearPlane;
  float2 Padding0;
};

cbuffer InstanceMode : register(b1)
{
  bool IsInstanceMode;
  float3 Padding1;
}

// VS Input
struct VS_INPUT
{
  // Layout
  float3 position : VERTEXPOS;
  float3 normal : NORMAL;
  float2 uv : UV;

  // Instancing
  float4x4 instanceMatrix : INSTANCE_TRANSFORM;
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

  matrix modelMatrix = IsInstanceMode ? input.instanceMatrix : Model;
  float4 worldPosition = mul(modelMatrix, float4(input.position, 1.0));

  output.position = mul(ViewProjection, worldPosition);
  output.worldpos = worldPosition.xyz;

  float3x3 normalMatrix = transpose((float3x3)modelMatrix);
  output.normal = normalize(mul(normalMatrix, input.normal));
  output.uv = input.uv;

  return output;
}