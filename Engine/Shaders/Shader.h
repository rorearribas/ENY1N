#pragma once
#include <d3d11.h>
#include <cassert>
#include "Engine/Render/RenderTypes.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace shader
  {
    template<EShader T>
    class CShader
    {
    public:
      CShader() {}
      ~CShader() { Release(); }

      CShader(const CShader&) = delete;
      CShader& operator=(const CShader&) = delete;

      HRESULT Init(const unsigned char* _pBuffer, size_t _tSize);
      void Release();

      void Attach();
      void Detach();

    private:
      ID3D11DeviceChild* m_pInternalPtr = nullptr;
    };

    template<EShader T>
    HRESULT render::shader::CShader<T>::Init(const unsigned char* _pBuffer, size_t _tSize)
    {
      ID3D11Device* pDevice = global::dx::s_pDevice;
#ifdef _DEBUG
      assert(pDevice);
#endif // DEBUG
      switch (T)
      {
        case render::EShader::E_VERTEX:
        {
          ID3D11VertexShader** pShader = reinterpret_cast<ID3D11VertexShader**>(&m_pInternalPtr);
          return pDevice->CreateVertexShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_HULL:
        {
          ID3D11HullShader** pShader = reinterpret_cast<ID3D11HullShader**>(&m_pInternalPtr);
          return pDevice->CreateHullShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_DOMAIN:
        {
          ID3D11DomainShader** pShader = reinterpret_cast<ID3D11DomainShader**>(&m_pInternalPtr);
          return pDevice->CreateDomainShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_GEOMETRY:
        {
          ID3D11GeometryShader** pShader = reinterpret_cast<ID3D11GeometryShader**>(&m_pInternalPtr);
          return pDevice->CreateGeometryShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_PIXEL:
        {
          ID3D11PixelShader** pShader = reinterpret_cast<ID3D11PixelShader**>(&m_pInternalPtr);
          return pDevice->CreatePixelShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_COMPUTE:
        {
          ID3D11ComputeShader** pShader = reinterpret_cast<ID3D11ComputeShader**>(&m_pInternalPtr);
          return pDevice->CreateComputeShader(_pBuffer, _tSize, nullptr, pShader);
        }
      }

      return E_FAIL;
    }

    template<EShader T>
    void CShader<T>::Release()
    {
      // Remove shader
      bool bOk(false);
      bOk = global::dx::SafeRelease(m_pInternalPtr);
#ifdef _DEBUG
      assert(bOk);
#endif // DEBUG
    }

    template<EShader T>
    void CShader<T>::Attach()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx::s_pDeviceContext;
#ifdef _DEBUG
      assert(pDeviceCtx);
#endif // DEBUG
      // Attach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { pDeviceCtx->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_HULL:     { pDeviceCtx->HSSetShader(reinterpret_cast<ID3D11HullShader*>(m_pInternalPtr),     nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { pDeviceCtx->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { pDeviceCtx->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(m_pInternalPtr), nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { pDeviceCtx->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(m_pInternalPtr),    nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { pDeviceCtx->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(m_pInternalPtr),  nullptr, 0); } break;
        default: break;
      }
    }

    template<EShader T>
    void CShader<T>::Detach()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx::s_pDeviceContext;
#ifdef _DEBUG
      assert(pDeviceCtx);
#endif // DEBUG
      // Detach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { pDeviceCtx->VSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_HULL:     { pDeviceCtx->HSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { pDeviceCtx->DSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { pDeviceCtx->GSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { pDeviceCtx->PSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { pDeviceCtx->CSSetShader(nullptr, nullptr, 0); } break;
        default: break;
      }
    }
  }
}
