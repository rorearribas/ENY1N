// Draw triangle - deferred pipeline
struct VS_OUTPUT
{
  float4 pos : SV_POSITION;
  float2 uv  : TEXCOORD0;
};

VS_OUTPUT VS_FullscreenTriangle(uint id : SV_VertexID)
{
  VS_OUTPUT output;
  float2 vertices[3] = { float2(-1.0, -1.0), float2(-1.0,  3.0), float2(3.0, -1.0) };
  output.pos = float4(vertices[id], 0.0, 1.0);
  output.uv = float2(0.5f * vertices[id].x + 0.5f, -0.5f * vertices[id].y + 0.5f);
  return output;
}