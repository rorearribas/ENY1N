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
	float y = uv.y * -2.0f + 1.0f;
    
	float4 position_v = mul(InvVP, float4(x, y, z, 1.0f));
	return position_v.xyz / position_v.w;
}

float2 get_uvs_from_light_space(float4 posLightSpace)
{
	float3 proj_coords = posLightSpace.xyz / posLightSpace.w;
	return proj_coords.xy * float2(0.5f, -0.5f) + 0.5f;
}

float2 texel_scale(Texture2D tex)
{
	uint width, height;
	tex.GetDimensions(width, height);
	return 1.0f / float2(max(1u, width), max(1u, height));
}

float offset_lookup(Texture2D tex, SamplerComparisonState tex_sampler, float2 uv, float2 offset, float current_depth)
{
	return tex.SampleCmpLevelZero(tex_sampler, uv + offset, current_depth);
}

float compute_shadow_mapping(Texture2D tex, SamplerComparisonState tex_sampler, float2 shadows_uv, float current_depth)
{
	if (shadows_uv.x < 0.0f || shadows_uv.x > 1.0f || shadows_uv.y < 0.0f || shadows_uv.y > 1.0f || current_depth > 1.0f)
	{
		return 1.0f;
	}

	float2 texelScale = texel_scale(tex);
	float fSum = 0.0f;

	[unroll]
	for (float y = -1.5f; y <= 1.5f; y += 1.0f)
	{
		[unroll]
		for (float x = -1.5f; x <= 1.5f; x += 1.0f)
		{
			fSum += offset_lookup(tex, tex_sampler, shadows_uv, float2(x, y) * texelScale, current_depth);
		}
	}
	
	float fShadowFactor = fSum / 16.0f;

	float2 v2DistFromCenter = abs(shadows_uv - 0.5f) * 2.0f;
	float fMaxDist = max(v2DistFromCenter.x, v2DistFromCenter.y);
	float fFadeStart = 0.85f;

	if (fMaxDist > fFadeStart)
	{
		float fFadeFactor = (fMaxDist - fFadeStart) / (1.0f - fFadeStart);
		fShadowFactor = lerp(fShadowFactor, 1.0f, saturate(fFadeFactor));
	}

	return fShadowFactor;
}