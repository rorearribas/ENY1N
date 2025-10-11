#pragma once

namespace render
{
  enum EShaderType : int32_t
  {
    E_VERTEX,
    E_HULL,
    E_DOMAIN,
    E_GEOMETRY,
    E_PIXEL,
    E_COMPUTE
  };

  enum EViewType : uint32_t
  {
    DEPTH_STENCIL,
    RENDER_TARGET,
    SHADER_RESOURCE,
    UNORDERED_ACCESS,
    UNKNOWN
  };

  // Types
  enum ETextureType : uint32_t
  {
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    EMISSIVE,
    HEIGHT,
    NORMAL,
    SHININESS,
    OPACITY,
    DISPLACEMENT,
    LIGHTMAP,
    REFLECTION,
    BASE_COLOR,
    NORMAL_CAMERA,
    EMISSION_COLOR,
    METALNESS,
    DIFFUSE_ROUGHNESS,
    AMBIENT_OCCLUSSION,
    COUNT
  };
}

