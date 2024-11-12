#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

struct CB_VS_vertexshader
{
  DirectX::XMMATRIX matrix;
};

template<class T>
class ConstantBuffer
{
private:
  ConstantBuffer(const ConstantBuffer<T>& rhs);

private:

  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
  ID3D11DeviceContext* deviceContext = nullptr;

public:
  ConstantBuffer() {}

  T data;

  ID3D11Buffer* Get()const
  {
    return pBuffer.Get();
  }

  ID3D11Buffer* const* GetAddressOf()const
  {
    return pBuffer.GetAddressOf();
  }

  HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
  {
    this->deviceContext = deviceContext;

    D3D11_BUFFER_DESC desc;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
    desc.StructureByteStride = 0;

    HRESULT hr = device->CreateBuffer(&desc, 0, pBuffer.GetAddressOf());
    return hr;
  }

  bool ApplyChanges()
  {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = this->deviceContext->Map(pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr))
    {
      return false;
    }
    CopyMemory(mappedResource.pData, &data, sizeof(T));
    this->deviceContext->Unmap(pBuffer.Get(), 0);
    return true;
  }
};