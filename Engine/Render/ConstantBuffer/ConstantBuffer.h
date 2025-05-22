#pragma once
#include <d3d11.h>
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"

// Model matrix
struct __declspec(align(16)) SConstantMatrix
{
  math::CMatrix4x4 Matrix = math::CMatrix4x4::Identity;
};

// Check model data
struct __declspec(align(16)) SConstantModelData
{
  // 8 Bytes
  bool bHasTexture = false;
  float Padding0;

  // 8 Bytes
  bool bHasModel = true;
  float Padding1;
};

#pragma region Lights
namespace internal
{
  // Directional lights
  struct __declspec(align(16)) SDirectionaLight
  {
    // 12 + 4 Bytes
    math::CVector3 Direction;
    float Padding0;
    // 12 + 4 bytes
    math::CVector3 Color;
    float Intensity;
  };
  // Point lights
  struct __declspec(align(16)) SPointLight
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
  struct __declspec(align(16)) SSpotLight
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
    // 12 + 4 Bytes
    float Range;
    float CutOffAngle;
    // 12 + 4 Bytes
    float Intensity;
    float Padding3;
  };
}

template<size_t MAX_POINT_LIGHTS, size_t MAX_SPOT_LIGHTS>
struct __declspec(align(16)) SGlobalLightningData
{
  // Lights [144 Bytes]
  internal::SDirectionaLight DirectionalLight;
  internal::SPointLight PointLights[MAX_POINT_LIGHTS];
  internal::SSpotLight SpotLights[MAX_SPOT_LIGHTS];
  // Handle lights [16 Bytes]
  int RegisteredPointLights;
  int RegisteredSpotLights;
  float Padding[2];
};

#pragma endregion

template<class T>
class CConstantBuffer
{
private:
  CConstantBuffer(const CConstantBuffer<T>& rhs);

public:
  CConstantBuffer() {}
  ~CConstantBuffer() { CleanBuffer(); }

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

  bool WriteBuffer()
  {
    D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
    HRESULT hResult = m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
    if (FAILED(hResult)) 
    { 
      return false; 
    }

    // Copy memory into buffer
    CopyMemory(oMappedSubresource.pData, &m_oData, sizeof(T));
    m_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }
  void CleanBuffer() { global::dx11::SafeRelease(m_pBuffer); }

  ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  T& GetData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData;
};