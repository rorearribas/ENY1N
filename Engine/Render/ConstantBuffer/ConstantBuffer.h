#pragma once
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"

template<class T>
class CConstantBuffer
{
public:
  CConstantBuffer() {}
  ~CConstantBuffer() { Clear(); }

  inline HRESULT Init()
  {
    D3D11_BUFFER_DESC oBufferDesc = D3D11_BUFFER_DESC();
    oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    oBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    oBufferDesc.MiscFlags = 0;
    oBufferDesc.StructureByteStride = 0;
    oBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(T) + 15) & ~15);

    return global::dx::s_pDevice->CreateBuffer(&oBufferDesc, 0, &m_pBuffer);
  }

  inline void Clear()
  {
    global::dx::SafeRelease(m_pBuffer);
  }

  template<render::EShader _Type>
  void Bind()
  {
    switch (_Type)
    {
      case render::EShader::E_VERTEX:   global::dx::s_pDeviceContext->VSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_HULL:     global::dx::s_pDeviceContext->HSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_DOMAIN:   global::dx::s_pDeviceContext->DSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_GEOMETRY: global::dx::s_pDeviceContext->GSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_PIXEL:    global::dx::s_pDeviceContext->PSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_COMPUTE:  global::dx::s_pDeviceContext->CSSetConstantBuffers(m_uSlot, 1, &m_pBuffer); break;
    }
  }
  inline bool WriteBuffer()
  {
    D3D11_MAPPED_SUBRESOURCE oMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
    HRESULT hResult = global::dx::s_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
    if (FAILED(hResult))
    {
      return false;
    }

    // Copy memory into buffer
    memcpy(oMappedSubresource.pData, &m_oData, sizeof(T));
    global::dx::s_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }

  inline void SetSlot(uint32_t _uSlot) { m_uSlot = _uSlot; }
  inline const uint32_t GetSlot() const { return m_uSlot; }

  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  inline T& GetData() { return m_oData; }

private:
  T m_oData = T();

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  uint32_t m_uSlot = 0;
};
