#pragma once
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

struct CBufferHandler
{
  uint32_t BeginOffset = 0;
  uint32_t EndOffset = 0;
  inline uint32_t GetOffset() const { return EndOffset - BeginOffset; }
};

template<class T>
class CRenderBuffer
{
public:
  CRenderBuffer() = default;
  ~CRenderBuffer() { Release(); }

  HRESULT Init(const D3D11_BUFFER_DESC& _rBufferDesc);
  void ResetOffset();
  void Release();

  bool Alloc(T* _pData, uint32_t _uCount, CBufferHandler& _rBufferHandler_);
  bool Free(const CBufferHandler& _rBufferHandler, uint32_t& _uLeftDisplacement_);

  inline operator ID3D11Buffer* () const { return m_pBuffer; }
  inline operator ID3D11Buffer* () { return m_pBuffer; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;

private:
  uint32_t m_uBufferSize = 0;
  uint32_t m_uCurrentOffset = 0;
};

template<class T>
void CRenderBuffer<T>::ResetOffset()
{
  m_uCurrentOffset = 0;
}

template<class T>
void CRenderBuffer<T>::Release()
{
  global::api::SafeRelease(m_pBuffer);
}

template<class T>
HRESULT CRenderBuffer<T>::Init(const D3D11_BUFFER_DESC& _rBufferDesc)
{
#ifdef _DEBUG
  assert(_rBufferDesc.Usage == D3D11_USAGE_DEFAULT);
#endif // _DEBUG
  HRESULT hResult = global::api::Device->CreateBuffer(&_rBufferDesc, nullptr, &m_pBuffer);
  if (!FAILED(hResult))
  {
    m_uBufferSize = _rBufferDesc.ByteWidth;
  }
  return hResult;
}

template<class T>
bool CRenderBuffer<T>::Alloc(T* _pData, uint32_t _uElements, CBufferHandler& _rBufferHandler_)
{
  uint32_t uTargetSize = (_uElements * sizeof(T));
  uint32_t uStartOffsetBytes = m_uCurrentOffset * sizeof(T);
  uint32_t uNextOffsetBytes = uStartOffsetBytes + uTargetSize;
  if (uNextOffsetBytes > m_uBufferSize)
  {
    ERROR_LOG("There isn't enough memory to allocate memory!");
    return false;
  }

  // Set data
  D3D11_BOX rDestBox = D3D11_BOX();
  rDestBox.left = uStartOffsetBytes;
  rDestBox.right = uNextOffsetBytes;
  rDestBox.top = 0; rDestBox.bottom = 1;
  rDestBox.front = 0; rDestBox.back = 1;

  // Update memory
  global::api::DeviceContext->UpdateSubresource(m_pBuffer, 0, &rDestBox, _pData, 0, 0);

  // Update values
  uint32_t uStartOffset = m_uCurrentOffset;
  m_uCurrentOffset += _uElements;

  // Set handler
  _rBufferHandler_.BeginOffset = uStartOffset;
  _rBufferHandler_.EndOffset = m_uCurrentOffset;

  return true;
}

template<class T>
bool CRenderBuffer<T>::Free(const CBufferHandler& _rBufferHandler, uint32_t& _uLeftDisplacement_)
{
  // Only if is the last assigned block
  if (_rBufferHandler.EndOffset == m_uCurrentOffset)
  {
    _uLeftDisplacement_ = 0;
    m_uCurrentOffset = _rBufferHandler.BeginOffset;
    return true;
  }

  // Calculate the gap size
  uint32_t uStartMoveBytes = _rBufferHandler.EndOffset * sizeof(T);
  uint32_t uEndMoveBytes = m_uCurrentOffset * sizeof(T);
  uint32_t uDestMoveBytes = _rBufferHandler.BeginOffset * sizeof(T);

  D3D11_BOX rSrcBox = D3D11_BOX();
  rSrcBox.left = uStartMoveBytes;
  rSrcBox.right = uEndMoveBytes;
  rSrcBox.top = 0; rSrcBox.bottom = 1;
  rSrcBox.front = 0; rSrcBox.back = 1;

  // Update memory
  global::api::DeviceContext->CopySubresourceRegion(m_pBuffer, 0, uDestMoveBytes, 0, 0, m_pBuffer, 0, &rSrcBox);

  // Update offset
  _uLeftDisplacement_ = (_rBufferHandler.EndOffset - _rBufferHandler.BeginOffset);
  m_uCurrentOffset -= _uLeftDisplacement_;

  return true;
}

