#pragma once
#include <d3d11.h>
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"

// Model matrix
struct __declspec(align(16)) SConstantMatrix
{
  maths::CMatrix4x4 mMatrix = maths::CMatrix4x4::Identity;
};

// Check textures
struct __declspec(align(16)) SConstantTexture
{
  bool bHasTexture = false;
  char padding[15];
};

#pragma region Lights
namespace internal
{
  // Directional lights
  struct SDirectionaLight
  {
    maths::CVector3 Direction;
    float Padding1;
    maths::CVector3 Color;
    float Intensity;
  };
  // Point lights
  struct SPointLight
  {
    maths::CVector3 Position;
    maths::CVector3 Color;
    float Intensity;
    float Range;
  };
  // Spot lights
  struct SSpotLight
  {
    maths::CVector3 Position;
    maths::CVector3 Direction;
    maths::CVector3 Color;
    float Intensity;
    float CutOffAngle;
  };
}

template<size_t MAX_POINT_LIGHTS, size_t MAX_SPOT_LIGHTS>
struct __declspec(align(16)) SGlobalLightningData
{
  internal::SDirectionaLight DirectionalLight;
  internal::SPointLight PointLights[MAX_POINT_LIGHTS];
  internal::SSpotLight SpotLights[MAX_SPOT_LIGHTS];
};

#pragma endregion

template<class T>
class CConstantBuffer
{
private:
  CConstantBuffer(const CConstantBuffer<T>& rhs);

public:
  CConstantBuffer() {}
  ~CConstantBuffer() {}

  HRESULT Init(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
  {
    m_pDeviceContext = _pDeviceContext;

    D3D11_BUFFER_DESC oBufferDesc;
    oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    oBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    oBufferDesc.MiscFlags = 0;
    oBufferDesc.StructureByteStride = 0;
    oBufferDesc.ByteWidth = static_cast<UINT>(sizeof(T));

    return _pDevice->CreateBuffer(&oBufferDesc, 0, &m_pBuffer);
  }

  bool UpdateBuffer()
  {
    D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
    HRESULT hr = m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
    if (FAILED(hr)) { return false; }
    // Copy memory into buffer
    CopyMemory(oMappedSubresource.pData, &m_oData, sizeof(T));
    m_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }
  void CleanBuffer()
  {
    global::dx11::SafeRelease(m_pBuffer);
  }

  ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  T& GetData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData;
};