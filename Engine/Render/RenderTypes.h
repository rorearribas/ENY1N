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
    static constexpr uint16_t s_uMaxMeshesPerModel = 4u;
    struct TVertexData
    {
      math::CVector3 VertexPos = math::CVector3::Zero;
      math::CVector3 Normal = math::CVector3::Zero;
      math::CVector3 Tangent = math::CVector3::Zero;
      math::CVector2 TexCoord = math::CVector2::Zero;

      inline bool operator==(const TVertexData& _rOther) const 
      { 
        return (VertexPos == _rOther.VertexPos) 
        && (Normal == _rOther.Normal) 
        && (Tangent == _rOther.Tangent) 
        && (TexCoord == _rOther.TexCoord); 
      }
      inline bool operator!=(const TVertexData& _rOther) const { return !(*this == _rOther); }
    };
    //------------------------------------------------
    //------------------INSTANCING--------------------
    //------------------------------------------------
    static constexpr uint16_t s_uMaxInstances = 128u;
    static constexpr uint16_t s_uMaxDrawableInstances = s_uMaxInstances - 1;
    typedef std::array<uint16_t, s_uMaxDrawableInstances> TDrawableInstances;
    struct TModelInstanceData
    {
      math::CMatrix4x4 Transform = math::CMatrix4x4::Identity;
    } static s_tModelInstanceData[s_uMaxInstances];
    //------------------------------------------------
    //------------------PRIMITIVES--------------------
    //------------------------------------------------
    struct TPrimitiveData
    {
      std::vector<math::CVector3> Vertices;
      std::vector<uint32_t> Indices;
    };
    struct TPrimitiveInstanceData
    {
      math::CMatrix4x4 Transform = math::CMatrix4x4::Identity;
      math::CVector3 Color = math::CVector3::Zero;
    } static s_tPrimitiveInstanceData[s_uMaxInstances];
  }

  // Render modes
  enum class ERenderMode : uint32_t
  {
    SOLID,
    WIREFRAME,
    INVALID
  };

  // Shader types
  enum class EShader : uint32_t
  {
    E_VERTEX,
    E_HULL,
    E_DOMAIN,
    E_GEOMETRY,
    E_PIXEL,
    E_COMPUTE
  };

  // View types
  enum class EView : uint32_t
  {
    DEPTH_STENCIL,
    RENDER_TARGET,
    SHADER_RESOURCE,
    UNORDERED_ACCESS,
    UNKNOWN
  };

  // Textures (Assimp)
  enum class ETexture : uint32_t
  {
    NONE,
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
  static constexpr uint32_t s_uTextureCount((uint32_t)(ETexture::COUNT));

  // Light types
  enum class ELight : uint32_t
  {
    DIRECTIONAL_LIGHT,
    POINT_LIGHT,
    SPOT_LIGHT,
    INVALID
  };

  // Primitive types
  enum class EPrimitive : uint32_t
  {
    E3D_CUBE,
    E3D_SPHERE,
    E3D_CAPSULE,
    E3D_PLANE,
    E2D_SQUARE,
    E2D_CIRCLE,
    E2D_TRIANGLE,
    CUSTOM,
    INVALID
  };
}

namespace std
{
  template <>
  struct hash<render::gfx::TVertexData>
  {
    // Algorithm from std::boost
    size_t operator()(const render::gfx::TVertexData& _rVertexData) const
    {
      size_t seed = 0;
      auto rFunc = [](float f) { return static_cast<int>(f * 1000); };
      auto hash_combine = [](size_t& s, size_t v) { s ^= v + 0x9e3779b9 + (s << 6) + (s >> 2); };

      hash_combine(seed, hash<int>()(rFunc(_rVertexData.VertexPos.x)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.VertexPos.y)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.VertexPos.z)));

      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Normal.x)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Normal.y)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Normal.z)));

      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Tangent.x)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Tangent.y)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.Tangent.z)));

      hash_combine(seed, hash<int>()(rFunc(_rVertexData.TexCoord.x)));
      hash_combine(seed, hash<int>()(rFunc(_rVertexData.TexCoord.y)));

      return seed;
    }
  };
}