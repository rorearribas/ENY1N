#pragma once
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"

template<class T>
class CRenderBuffer
{
public:
  HRESULT Init(const D3D11_BUFFER_DESC& _rBufferDesc);
  void Release();

  bool Alloc(T* _pData, uint32_t _uCount, uint32_t& _uStartOffset_);
  bool Dealloc();

  inline ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  inline const uint32_t& GetCurrentOffset() const { return m_uCurrentOffset; }

  inline operator ID3D11Buffer* () const { return m_pBuffer; }
  inline operator ID3D11Buffer* () { return m_pBuffer; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  uint32_t m_uCurrentOffset = 0;
  uint32_t m_uMaxBufferSize = 0;
};

template<class T>
HRESULT CRenderBuffer<T>::Init(const D3D11_BUFFER_DESC& _rBufferDesc)
{
  m_uMaxBufferSize = _rBufferDesc.ByteWidth;
  return global::api::Device->CreateBuffer(&_rBufferDesc, nullptr, &m_pBuffer);
}

template<class T>
void CRenderBuffer<T>::Release()
{
  global::api::SafeRelease(m_pBuffer);
}

template<class T>
bool CRenderBuffer<T>::Alloc(T* _pData, uint32_t _uCount, uint32_t& _uStartOffset_)
{
  uint32_t uTargetSize = _uCount * sizeof(T);
  uint32_t uNextOffset = m_uCurrentOffset * sizeof(T) + uTargetSize;
  if (uNextOffset > m_uMaxBufferSize)
  {
    ERROR_LOG("There isn't enough memory to allocate memory!");
    return false;
  }

  // Map buffer
  D3D11_MAPPED_SUBRESOURCE rMappedSubresource = D3D11_MAPPED_SUBRESOURCE();
  HRESULT hResult = global::api::DeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &rMappedSubresource);
  if (FAILED(hResult))
  {
    ERROR_LOG("Error mapping buffer!");
    return false;
  }

  uint32_t uStartOffset = m_uCurrentOffset;
  T* pCurrentData = static_cast<T*>(rMappedSubresource.pData);
  T* pWritePtr = pCurrentData + uStartOffset;
  memcpy(pWritePtr, _pData, uTargetSize);

  // Set offsets
  _uStartOffset_ = m_uCurrentOffset;
  m_uCurrentOffset += _uCount;

  // Unmap
  global::api::DeviceContext->Unmap(m_pBuffer, 0);

  return true;
}

template<class T>
bool CRenderBuffer<T>::Dealloc()
{
  m_uCurrentOffset = 0;
  return true;
}

