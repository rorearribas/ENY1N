// Standard vertex shader - ENY1N

cbuffer cbTransforms : register(b0)
{
  // Transforms
	matrix ViewProjection;
	matrix InvViewProjection;
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
		// Set world position and transform to clip space
		float4 worldPosition = mul(input.modelMatrix, float4(input.position, 1.0));
		output.position = mul(ViewProjection, worldPosition);
		output.worldpos = worldPosition.xyz;

		// Normalize the normal using the model matrix to account for scaling and rotation
		float3 col0 = normalize(float3(input.modelMatrix[0].x, input.modelMatrix[1].x, input.modelMatrix[2].x));
		float3 col1 = normalize(float3(input.modelMatrix[0].y, input.modelMatrix[1].y, input.modelMatrix[2].y));
		float3 col2 = normalize(float3(input.modelMatrix[0].z, input.modelMatrix[1].z, input.modelMatrix[2].z));
		float3x3 normalizedMatrix = float3x3(col0, col1, col2);
		output.normal = normalize(mul(normalizedMatrix, input.normal));
				
		// Set uv coordinates
		output.uv = input.uv;
	}
	return output;
}