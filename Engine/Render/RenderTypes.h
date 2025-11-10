#pragma once

namespace render
{
  // Render modes
  enum class ERenderMode
  {   
    SOLID, 
    WIREFRAME 
  };

  // Shader types
  enum class EShaderType
  {
    E_VERTEX,
    E_HULL,
    E_DOMAIN,
    E_GEOMETRY,
    E_PIXEL,
    E_COMPUTE
  };

  // View types
  enum class EViewType
  {
    DEPTH_STENCIL,
    RENDER_TARGET,
    SHADER_RESOURCE,
    UNORDERED_ACCESS,
    UNKNOWN
  };

  // Textures types
  enum class ETextureType
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

  // Light types
  enum class ELightType
  {
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
    INVALID
  };
  
  // Primitive types
  enum class EPrimitiveType
  {
    E2D_SQUARE,
    E2D_CIRCLE,
    E2D_TRIANGLE,
    E3D_PLANE,
    E3D_CUBE,
    E3D_SPHERE,
    E3D_CAPSULE,
    CUSTOM,
    INVALID
  };
}

