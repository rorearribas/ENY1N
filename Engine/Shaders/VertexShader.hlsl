// Vertex Shader

/* vertex attributes go here to input to the vertex shader */
struct vs_in 
{
    float3 local_position : POSITION;
    float3 color : COLOR;
};

/* outputs from vertex shader go here. can be interpolated to pixel shader */
struct vs_out 
{
    float4 clip_position : SV_POSITION; // required output of VS
    float4 color : COLOR;
};

vs_out vs_main(vs_in input) 
{
  vs_out output = (vs_out)0; // zero the memory first
  output.clip_position = float4(input.local_position, 1.0);
  output.color = float4(input.color, 1.0);
  return output;
}

