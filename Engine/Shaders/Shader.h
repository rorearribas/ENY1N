#pragma once
#include <d3d11.h>
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace shader
  {
    enum EShaderType : int32_t
    {
      E_VERTEX,
      E_HULL,
      E_DOMAIN,
      E_GEOMETRY,
      E_PIXEL,
      E_COMPUTE
    };

    template<EShaderType T>
    class CShader
    {
    public:
      CShader(const unsigned char* _pBuffer, size_t _tSize);
      ~CShader() { ReleaseShader(); }

      void AttachShader();
      void DetachShader();
      void ReleaseShader();

    private:
      ID3D11DeviceChild* m_pInternalPtr = nullptr;
    };

    template<EShaderType T>
    void CShader<T>::ReleaseShader()
    {
      // Remove shader
      bool bOk(false);
      bOk = global::dx::SafeRelease(m_pInternalPtr);
      assert(bOk);
    }

    template<EShaderType T>
    void CShader<T>::AttachShader()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx::s_pDeviceContext;
      assert(pDeviceCtx);
      // Attach shader
      switch (T)
      {
        case EShaderType::E_VERTEX:   { pDeviceCtx->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case EShaderType::E_HULL:     { pDeviceCtx->HSSetShader(reinterpret_cast<ID3D11HullShader*>(m_pInternalPtr),     nullptr, 0); } break;
        case EShaderType::E_DOMAIN:   { pDeviceCtx->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case EShaderType::E_GEOMETRY: { pDeviceCtx->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(m_pInternalPtr), nullptr, 0); } break;
        case EShaderType::E_PIXEL:    { pDeviceCtx->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(m_pInternalPtr),    nullptr, 0); } break;
        case EShaderType::E_COMPUTE:  { pDeviceCtx->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(m_pInternalPtr),  nullptr, 0); } break;
        default: break;
      }
    }

    template<EShaderType T>
    void CShader<T>::DetachShader()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx::s_pDeviceContext;
      assert(pDeviceCtx);
      // Detach shader
      switch (T)
      {
        case EShaderType::E_VERTEX:    { pDeviceCtx->VSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::E_HULL:      { pDeviceCtx->HSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::E_DOMAIN:    { pDeviceCtx->DSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::E_GEOMETRY:  { pDeviceCtx->GSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::E_PIXEL:     { pDeviceCtx->PSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::E_COMPUTE:   { pDeviceCtx->CSSetShader(nullptr, nullptr, 0); } break;
        default: break;
      }
    }

    template<EShaderType T>
    CShader<T>::CShader(const unsigned char* _pBuffer, size_t _tSize)
    {
      ID3D11Device* _pDevice = global::dx::s_pDevice;
      assert(_pDevice);

      HRESULT hResult = S_OK;
      switch (T)
      {
      case EShaderType::E_VERTEX:
      {
        ID3D11VertexShader** pShader = reinterpret_cast<ID3D11VertexShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateVertexShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      case EShaderType::E_HULL:
      {
        ID3D11HullShader** pShader = reinterpret_cast<ID3D11HullShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateHullShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      case EShaderType::E_DOMAIN:
      {
        ID3D11DomainShader** pShader = reinterpret_cast<ID3D11DomainShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateDomainShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      case EShaderType::E_GEOMETRY:
      {
        ID3D11GeometryShader** pShader = reinterpret_cast<ID3D11GeometryShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateGeometryShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      case EShaderType::E_PIXEL:
      {
        ID3D11PixelShader** pShader = reinterpret_cast<ID3D11PixelShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreatePixelShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      case EShaderType::E_COMPUTE:
      {
        ID3D11ComputeShader** pShader = reinterpret_cast<ID3D11ComputeShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateComputeShader(_pBuffer, _tSize, nullptr, pShader);
      }
      break;
      }
#ifdef _DEBUG
      assert(SUCCEEDED(hResult));
#endif
    }
  }
}
