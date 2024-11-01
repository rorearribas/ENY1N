#include "VertexShader.hlsl"

float4 ps_main(vs_out input) : SV_TARGET 
{
  return input.color;
}