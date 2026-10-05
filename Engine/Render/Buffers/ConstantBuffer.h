#pragma once
#include "Libs/Math/Matrix4x4.h"
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/RenderContext/RenderCommandsDX11.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"

template<class T>
class CConstantBuffer
{
public:
  CConstantBuffer() = default;
  ~CConstantBuffer() { Release(); }

  HRESULT Init(const render::CRenderDeviceDX11& _rRenderDevice);
  void Release();

  template<render::EShader _Type>
  inline void Bind(const render::CRenderCommandsDX11& _rCommands, uint32_t _uSlot)
  {
    switch (_Type)
    {
      case render::EShader::E_VERTEX:   _rCommands->VSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_HULL:     _rCommands->HSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_DOMAIN:   _rCommands->DSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_GEOMETRY: _rCommands->GSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_PIXEL:    _rCommands->PSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
      case render::EShader::E_COMPUTE:  _rCommands->CSSetConstantBuffers(_uSlot, 1, &m_pBuffer); break;
    }
  }
  bool WriteBuffer(const render::CRenderCommandsDX11& _rCommands, const T& _rData);
  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
};

template<class T>
HRESULT CConstantBuffer<T>::Init(const render::CRenderDeviceDX11& _rRenderDevice)
{
  D3D11_BUFFER_DESC rBufferDesc = D3D11_BUFFER_DESC();
  rBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  rBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  rBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  rBufferDesc.MiscFlags = 0u;
  rBufferDesc.StructureByteStride = 0u;
  rBufferDesc.ByteWidth = static_cast<uint32_t>((sizeof(T) + 15) & ~15);
  return _rRenderDevice->CreateBuffer(&rBufferDesc, 0, &m_pBuffer);
}

template<class T>
void CConstantBuffer<T>::Release()
{
  global::api::SafeRelease(m_pBuffer);
}

template<class T>
bool CConstantBuffer<T>::WriteBuffer(const render::CRenderCommandsDX11& _rCommands, const T& _rData)
{
  D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
  HRESULT hResult = _rCommands->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &rMappedSubresource);
  if (FAILED(hResult))
  {
    return false;
  }

  // Copy memory into buffer
  memcpy(rMappedSubresource.pData, &_rData, sizeof(T));
  _rCommands->Unmap(m_pBuffer, 0);
  return true;
}