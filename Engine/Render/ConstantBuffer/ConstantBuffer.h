#pragma once
#include <d3d11.h>
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"

static int constexpr s_iAlignMemory = 16;
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

// Matrix
struct __declspec(align(s_iAlignMemory)) SConstantTransforms
{
  // MVP
  math::CMatrix4x4 View = math::CMatrix4x4::Identity;
  math::CMatrix4x4 Projection = math::CMatrix4x4::Identity;
  math::CMatrix4x4 Model = math::CMatrix4x4::Identity;
  // Inverse
  math::CMatrix4x4 InvView = math::CMatrix4x4::Identity;
  math::CMatrix4x4 InvProjection = math::CMatrix4x4::Identity;
  // Projection CFG
  float FarPlane = 10000.0f;
  float NearPlane = 0.01f;
  float Padding[2];
};

// Textures data
struct __declspec(align(s_iAlignMemory)) STexturesData
{
  int HasDiffuse;
  int HasNormal;
  int HasSpecular;
  int Padding;
};

// Directional lights
struct __declspec(align(s_iAlignMemory)) SDirectionaLight
{
  // 12 + 4 Bytes
  math::CVector3 Direction;
  float Padding0;
  // 12 + 4 bytes
  math::CVector3 Color;
  float Intensity;
};
// Point lights
struct __declspec(align(s_iAlignMemory)) SPointLight
{
  // 12 + 4 Bytes
  math::CVector3 Position;
  float Padding0;
  // 12 + 4 Bytes
  math::CVector3 Color;
  float Padding1;
  // 12 + 4 Bytes
  float Range;
  float Intensity;
  // 12 + 4 Bytes
  float Padding[2];
};
// Spot lights
struct __declspec(align(s_iAlignMemory)) SSpotLight
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
struct __declspec(align(s_iAlignMemory)) SGlobalLightingData
{
  // Lights [144 Bytes]
  SDirectionaLight DirectionalLight;
  SPointLight PointLights[MAX_POINT_LIGHTS];
  SSpotLight SpotLights[MAX_SPOT_LIGHTS];

  // Handle lights [16 Bytes]
  int RegisteredPointLights;
  int RegisteredSpotLights;
  float Padding[2];
};

template<class T>
class CConstantBuffer
{
private:
  CConstantBuffer(const CConstantBuffer<T>& rhs);

public:
  CConstantBuffer() {}
  ~CConstantBuffer() { Clear(); }

  HRESULT Init(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
  {
    m_pDeviceContext = _pDeviceContext;

    D3D11_BUFFER_DESC oBufferDesc = D3D11_BUFFER_DESC();
    oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    oBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    oBufferDesc.MiscFlags = 0;
    oBufferDesc.StructureByteStride = 0;
    oBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(T) + 15) & ~15);

    return _pDevice->CreateBuffer(&oBufferDesc, 0, &m_pBuffer);
  }

  inline bool WriteBuffer()
  {
    D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
    HRESULT hResult = m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
    if (FAILED(hResult))
    {
      return false;
    }

    // Copy memory into buffer
    memcpy(oMappedSubresource.pData, &m_oData, sizeof(T));
    m_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }
  inline void Clear() { global::dx::SafeRelease(m_pBuffer); }

  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  inline T& GetData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData = T();
};