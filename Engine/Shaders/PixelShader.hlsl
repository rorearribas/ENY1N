#include "VertexShader.hlsl"

float4 ps_main(vs_out input) : SV_TARGET 
{
  return float4( 1.0, 1.0, 1.0, 1.0 ); // must return an RGBA colour
}