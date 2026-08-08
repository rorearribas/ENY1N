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
  {
		// Calculate world pos and transform to clip space
		float4 worldPosition = mul(input.instanceMatrix, float4(input.position, 1.0));
		output.position = mul(ViewProjection, worldPosition);
		output.worldpos = worldPosition.xyz;
		
		// Calculate normal (it doesn't work with negative scales)
		float3x3 instanceMatrix = (float3x3) input.instanceMatrix;
		float3 scale;
		{
			scale.x = length(instanceMatrix[0].xyz);
			scale.y = length(instanceMatrix[1].xyz);
			scale.z = length(instanceMatrix[2].xyz);
		}
		output.normal = normalize(mul(instanceMatrix, (input.normal / (scale * scale))));
				
		// Set uv coordinates
		output.uv = input.uv;
	}
	return output;
}