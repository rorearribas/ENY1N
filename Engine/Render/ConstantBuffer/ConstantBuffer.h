#pragma once
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"

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

  void Bind(uint32_t _uSlot, render::EShaderType _eShaderType);
  bool WriteBuffer();

  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  inline void Clear() { global::dx::SafeRelease(m_pBuffer); }
  inline T& GetData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData = T();
};

template<class T>
void CConstantBuffer<T>::Bind(uint32_t _uSlot, render::EShaderType _eShaderType)
{
  switch (_eShaderType)
  {
    case render::EShaderType::E_VERTEX:   global::dx::s_pDeviceContext->VSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    case render::EShaderType::E_HULL:     global::dx::s_pDeviceContext->HSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    case render::EShaderType::E_DOMAIN:   global::dx::s_pDeviceContext->DSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    case render::EShaderType::E_GEOMETRY: global::dx::s_pDeviceContext->GSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    case render::EShaderType::E_PIXEL:    global::dx::s_pDeviceContext->PSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    case render::EShaderType::E_COMPUTE:  global::dx::s_pDeviceContext->CSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
  }
}

template<class T>
bool CConstantBuffer<T>::WriteBuffer()
{
  D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
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
