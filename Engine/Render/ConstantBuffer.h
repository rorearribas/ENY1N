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

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.ByteWidth = static_cast<UINT>(sizeof(maths::CMatrix4x4) + (16 - (sizeof(maths::CMatrix4x4) % 16)));

    HRESULT hr = _pDevice->CreateBuffer(&desc, 0, &m_pBuffer);
    return hr;
  }

  bool Apply()
  {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = this->m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
      return false;
    }
    CopyMemory(mappedResource.pData, &m_oData, sizeof(T));
    this->m_pDeviceContext->Unmap(m_pBuffer, 0);
    return true;
  }

  ID3D11Buffer* GetBuffer() const { return m_pBuffer; }
  T& GetCurrentData() { return m_oData; }

private:
  ID3D11Buffer* m_pBuffer = nullptr;
  ID3D11DeviceContext* m_pDeviceContext = nullptr;
  T m_oData;
};