// Lighting vertex shader - ENY1N
cbuffer cbLightingView : register(b0)
{
	float3 LightCameraPos;
	float Padding2;
	
	matrix LightViewProjection;
	matrix InvLightViewProjection;
}

// Vertex shader Input
struct VS_INPUT
{
  // Vertex info
	float3 position : VERTEXPOS;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : UV;

  // Instancing
	float4x4 instanceMatrix : INSTANCE_TRANSFORM;
};

// Pixel shader input shadow
struct PS_INPUT_LIGHTING
{
  float4 position : SV_POSITION;
};

PS_INPUT_LIGHTING Lighting_VSMain(VS_INPUT input)
{
	PS_INPUT_LIGHTING output;
  {
		float4 worldPosition = mul(input.instanceMatrix, float4(input.position, 1.0));
    output.position = mul(LightViewProjection, worldPosition);
  }
  return output;
}