// Deferred
struct PS_INPUT
{
  float4 position : SV_POSITION;
  float3 worldpos : POSITION;
  float3 normal : NORMAL;
  float3 color: COLOR;
  float2 uv : UV;
};

struct GBuffer 
{
  float4 diffuse : SV_TARGET0;
  float4 normal : SV_TARGET1;
  float4 position : SV_TARGET2;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  // Get back pixel color
  return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
