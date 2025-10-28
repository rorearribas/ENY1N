#include "SimpleVS.hlsl" 

float4 PSMain(PS_INPUT input) : SV_TARGET
{
  return float4(input.color, 1.0f);
}
