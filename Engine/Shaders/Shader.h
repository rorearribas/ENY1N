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
      switch (T)
      {
        case render::EShader::E_VERTEX:
        {
          ID3D11VertexShader** pShader = reinterpret_cast<ID3D11VertexShader**>(&m_pInternalPtr);
          return global::api::Device->CreateVertexShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_HULL:
        {
          ID3D11HullShader** pShader = reinterpret_cast<ID3D11HullShader**>(&m_pInternalPtr);
          return global::api::Device->CreateHullShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_DOMAIN:
        {
          ID3D11DomainShader** pShader = reinterpret_cast<ID3D11DomainShader**>(&m_pInternalPtr);
          return global::api::Device->CreateDomainShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_GEOMETRY:
        {
          ID3D11GeometryShader** pShader = reinterpret_cast<ID3D11GeometryShader**>(&m_pInternalPtr);
          return global::api::Device->CreateGeometryShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_PIXEL:
        {
          ID3D11PixelShader** pShader = reinterpret_cast<ID3D11PixelShader**>(&m_pInternalPtr);
          return global::api::Device->CreatePixelShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_COMPUTE:
        {
          ID3D11ComputeShader** pShader = reinterpret_cast<ID3D11ComputeShader**>(&m_pInternalPtr);
          return global::api::Device->CreateComputeShader(_pBuffer, _tSize, nullptr, pShader);
        }
      }

      return E_FAIL;
    }

    template<EShader T>
    void CShader<T>::Release()
    {
      // Remove shader
      bool bOk(false);
      bOk = global::api::SafeRelease(m_pInternalPtr);
#ifdef _DEBUG
      assert(bOk);
#endif // DEBUG
    }

    template<EShader T>
    void CShader<T>::Attach()
    {
      // Attach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { global::api::DeviceContext->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_HULL:     { global::api::DeviceContext->HSSetShader(reinterpret_cast<ID3D11HullShader*>(m_pInternalPtr),     nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { global::api::DeviceContext->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { global::api::DeviceContext->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(m_pInternalPtr), nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { global::api::DeviceContext->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(m_pInternalPtr),    nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { global::api::DeviceContext->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(m_pInternalPtr),  nullptr, 0); } break;
        default: break;
      }
    }

    template<EShader T>
    void CShader<T>::Detach()
    {
      // Detach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { global::api::DeviceContext->VSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_HULL:     { global::api::DeviceContext->HSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { global::api::DeviceContext->DSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { global::api::DeviceContext->GSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { global::api::DeviceContext->PSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { global::api::DeviceContext->CSSetShader(nullptr, nullptr, 0); } break;
        default: break;
      }
    }
  }
}
