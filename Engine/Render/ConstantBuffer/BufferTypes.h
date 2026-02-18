#pragma once
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"

// Target align
static constexpr uint32_t s_uAlign = 16;

/* Reference table
| HLSL Type       | Size (bytes)   | Equivalent
|-----------------|----------------|------------------------
| bool            | 1              | bool
| int             | 4              | int
| uint            | 4              | unsigned int
| float           | 4              | float
| float2          | 8              | math::CVector2
| float3          | 12             | math::CVector3
| float4          | 16             | math::CVector4
| float3x3        | 48             | math::CVector3x3
| float4x4        | 64             | math::CMatrix4x4
| matrix NxM      | N×16 bytes     |
*/

// Transforms [208 Bytes]
struct __declspec(align(s_uAlign)) TTransforms
{
  // Transforms
  math::CMatrix4x4 ViewProjection = math::CMatrix4x4::Identity;
  math::CMatrix4x4 InvViewProjection = math::CMatrix4x4::Identity;

  // Projection CFG
  float FarPlane = 10000.0f;
  float NearPlane = 0.01f;
  float Padding[2];
};

// Material info [48 Bytes]
struct __declspec(align(s_uAlign)) TMaterialInfo
{
  // 12 + 4 Bytes
  math::CVector3 DiffuseColor;
  int HasDiffuseTexture;

  // 12 + 4 Bytes
  math::CVector3 SpecularColor;
  int HasSpecularTexture;

  // 4 + 12 Bytes;
  int HasNormalTexture;
  int Padding[3];
};

// TLightView [16 Bytes]
struct __declspec(align(s_uAlign)) TLightView
{
  math::CMatrix4x4 LightViewProjection = math::CMatrix4x4::Identity;
};

// Directional lights [32 Bytes]
struct __declspec(align(s_uAlign)) TDirectionalLight
{
  // 12 + 4 Bytes
  math::CVector3 Direction;
  float Padding0;
  // 12 + 4 bytes
  math::CVector3 Color;
  float Intensity;
};

// Point lights [48 Bytes]
struct __declspec(align(s_uAlign)) TPointLight
{
  // 12 + 4 Bytes
  math::CVector3 Position;
  float Padding0;
  // 12 + 4 Bytes
  math::CVector3 Color;
  float Padding1;
  // 4 + 4 + 8 Bytes
  float Range;
  float Intensity;
  float Padding[2];
};
// Spot lights [64 Bytes]
struct __declspec(align(s_uAlign)) TSpotLight
{
  // 12 + 4 Bytes
  math::CVector3 Position;
  float Padding0;
  // 12 + 4 Bytes
  math::CVector3 Direction;
  float Padding1;
  // 12 + 4 Bytes
  math::CVector3 Color;
  float Padding2;
  // 4 + 4 + 8 Bytes
  float Range;
  float Intensity;
  float Padding3[2];
};

template<size_t MAX_POINT_LIGHTS, size_t MAX_SPOT_LIGHTS>
struct __declspec(align(s_uAlign)) TGlobalLighting
{
  // Lights [144 Bytes]
  TDirectionalLight DirectionalLight;
  TPointLight PointLights[MAX_POINT_LIGHTS];
  TSpotLight SpotLights[MAX_SPOT_LIGHTS];

  // Handle lights [16 Bytes]
  int RegisteredPointLights;
  int RegisteredSpotLights;
  float Padding[2];
};