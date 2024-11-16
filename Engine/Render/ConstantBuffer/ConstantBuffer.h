#pragma once
#include <d3d11.h>
#include "Libs/Maths/Matrix4x4.h"

struct SConstantBuffer
{
  maths::CMatrix4x4 mMatrix = maths::CMatrix4x4::Identity;
};

template<class T>
class ConstantBuffer
{
private:
  ConstantBuffer(const ConstantBuffer<T>& rhs);

public:
  ConstantBuffer() {}

  HRESULT Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext)
  {
    m_pDeviceContext = _pDeviceContext;

    D3D11_BUFFER_DESC oBufferDesc;
    oBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    oBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    oBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    oBufferDesc.MiscFlags = 0;
    oBufferDesc.StructureByteStride = 0;
    oBufferDesc.ByteWidth = static_cast<UINT>(sizeof(maths::CMatrix4x4) + (16 - (sizeof(maths::CMatrix4x4) % 16)));

    HRESULT hr = _pDevice->CreateBuffer(&oBufferDesc, 0, &m_pBuffer);
    return hr;
  }

  bool UpdateBuffer()
  {
    D3D11_MAPPED_SUBRESOURCE oMappedSubresource;
    HRESULT hr = m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &oMappedSubresource);
    if (FAILED(hr))
    {
      return false;
    }
    CopyMemory(oMappedSubresource.pData, &m_oData, sizeof(T));
    m_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }

  ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  T& GetData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData;
};