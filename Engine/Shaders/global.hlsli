//////////////////////////////////////////////
//////////// GLOBAL FUNCTIONS ////////////////
//////////////////////////////////////////////

float3 unpack_normal(float3 _v3Normal)
{
  return _v3Normal * 2.0f - 1.0f;
}

float get_linear_depth(float near, float far, float depth)
{
  return (2.0f * near) / (far + near - depth * (far - near));
}

float3 get_pos_from_depth(in float2 uv, in float z, in float4x4 InvVP)
{
  float x = uv.x * 2.0f - 1.0f;
  float y = (1.0 - uv.y) * 2.0f - 1.0f;
  float4 position_v = mul(InvVP, float4(x, y, z, 1.0f));
  return position_v.xyz / position_v.w;
}

float2 get_uvs_from_light_space(float4 posLightSpace)
{
  float3 proj_coords = posLightSpace.xyz / posLightSpace.w;
  float2 shadow_uv = proj_coords.xy * 0.5f + 0.5f;
  return float2(shadow_uv.x, 1.0f - shadow_uv.y);
}

float2 texture_size(Texture2D tex)
{
  uint width, height;
  tex.GetDimensions(width, height);
  return float2(width, height);
}

float2 texel_scale(Texture2D tex)
{
  return 1.0f / texture_size(tex);
}

float3 offset_lookup(Texture2D tex, SamplerComparisonState tex_sampler, float2 uv, float2 offset, float2 texel_size, float current_depth)
{
  return tex.SampleCmpLevelZero(tex_sampler, uv + offset * texel_size, current_depth);
}

float compute_shadow_mapping(Texture2D tex, SamplerComparisonState tex_sampler, float2 shadows_uv, float current_depth, uint samples)
{
  float2 texelScale = texel_scale(tex);

  float fSum = 0.0f;
  for (float y = -1.5; y <= 1.5; y += 1.0)
  {
    for (float x = -1.5; x <= 1.5; x += 1.0)
    {
      fSum += offset_lookup(tex, tex_sampler, shadows_uv, texelScale, float2(x, y), current_depth);
    }
  }
  return fSum / samples;
}