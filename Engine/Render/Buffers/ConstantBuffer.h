#pragma once
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"

template<class T>
class CConstantBuffer
{
public:
  CConstantBuffer() {}
  ~CConstantBuffer() { Release(); }

  HRESULT Init();
  void Release();

  template<render::EShader _Type>
  inline void Bind(uint32_t _uSlot)
  {
    switch (_Type)
    {
      case render::EShader::E_VERTEX:   global::api::DeviceContext->VSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_HULL:     global::api::DeviceContext->HSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_DOMAIN:   global::api::DeviceContext->DSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_GEOMETRY: global::api::DeviceContext->GSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_PIXEL:    global::api::DeviceContext->PSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_COMPUTE:  global::api::DeviceContext->CSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    }
  }
  bool WriteBuffer(const T& _rData);
  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
};

template<class T>
HRESULT CConstantBuffer<T>::Init()
{
  D3D11_BUFFER_DESC rBufferDesc = D3D11_BUFFER_DESC();
  rBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  rBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  rBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  rBufferDesc.MiscFlags = 0;
  rBufferDesc.StructureByteStride = 0;
  rBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(T) + 15) & ~15);

  return global::api::Device->CreateBuffer(&rBufferDesc, 0, &m_pBuffer);
}

template<class T>
void CConstantBuffer<T>::Release()
{
  global::api::SafeRelease(m_pBuffer);
}

template<class T>
bool CConstantBuffer<T>::WriteBuffer(const T& _rData)
{
  D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
  HRESULT hResult = global::api::DeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
  if (FAILED(hResult))
  {
    return false;
  }

  // Copy memory into buffer
  memcpy(rMappedSubresource.pData, &_rData, sizeof(T));
  global::api::DeviceContext->Unmap(m_pBuffer, 0);
  return true;
}