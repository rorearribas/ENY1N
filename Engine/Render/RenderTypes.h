#pragma once
#include "Libs/Math/Vector2.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Matrix4x4.h"

namespace render
{
  namespace gfx
  {
    //------------------------------------------------
    //-------------------MODELS-----------------------
    //------------------------------------------------
    struct TVertexData
    {
      math::CVector3 VertexPos = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector2 TexCoord = math::CVector2::Zero;

      inline bool operator==(const TVertexData& _other) const { return VertexPos == _other.VertexPos && Normal == _other.Normal && TexCoord == _other.TexCoord; }
      inline bool operator!=(const TVertexData& _other) const { return !(*this == _other); }
    };

    //------------------------------------------------
    //------------------INSTANCING--------------------
    //------------------------------------------------
    static constexpr uint32_t s_uMaxModelInstances = 10000u;
    typedef std::array<uint32_t, s_uMaxModelInstances> TDrawableInstances;
    struct TInstanceData
    {
      math::CMatrix4x4 Transform = math::CMatrix4x4::Identity;
    };

    //------------------------------------------------
    //------------------PRIMITIVES--------------------
    //------------------------------------------------
    struct TPrimitiveData
    {
      math::CVector3 VertexPos = math::CVector3::Zero;
      math::CVector3 Color = math::CVector3::Zero;
    };

    struct TCustomPrimitive
    {
      std::vector<render::gfx::TPrimitiveData> PrimitiveData;
      std::vector<uint32_t> Indices;
    };
  }

  // Render modes
  enum class ERenderMode : uint32_t
  {
    SOLID,
    WIREFRAME
  };

  // Shader types
  enum class EShaderType : uint32_t
  {
    E_VERTEX,
    E_HULL,
    E_DOMAIN,
    E_GEOMETRY,
    E_PIXEL,
    E_COMPUTE
  };

  // View types
  enum class EViewType : uint32_t
  {
    DEPTH_STENCIL,
    RENDER_TARGET,
    SHADER_RESOURCE,
    UNORDERED_ACCESS,
    UNKNOWN
  };

  // Textures types
  enum class ETextureType : uint32_t
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
  enum class ELightType : uint32_t
  {
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
    INVALID
  };

  // Primitive types
  enum class EPrimitiveType : uint32_t
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

namespace std
{
  template <>
  struct hash<render::gfx::TVertexData>
  {
    size_t operator()(const render::gfx::TVertexData& v) const
    {
      auto oFunc = [](float f) { return static_cast<int>(f * 1000); };
      size_t h1 = hash<int>()(oFunc(v.VertexPos.x)) ^ hash<int>()(oFunc(v.VertexPos.y)) ^ hash<int>()(oFunc(v.VertexPos.z));
      size_t h2 = hash<int>()(oFunc(v.Normal.x)) ^ hash<int>()(oFunc(v.Normal.y)) ^ hash<int>()(oFunc(v.Normal.z));
      size_t h3 = hash<int>()(oFunc(v.TexCoord.x)) ^ hash<int>()(oFunc(v.TexCoord.y));
      return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
  };
}