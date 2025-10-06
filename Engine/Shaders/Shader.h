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
      VERTEX_SHADER,
      PIXEL_SHADER,
      COMPUTE_SHADER,
      INVALID = -1
    };

    template<EShaderType eShaderType>
    class CShader
    {
    public:
      CShader(const BYTE* _pData, size_t _tSize);
      ~CShader() { ReleaseShader(); }

      void PushShader();
      void DetachShader();
      void ReleaseShader();

    private:
      ID3D11DeviceChild* m_pInternalPtr = nullptr;
    };

    template<EShaderType eShaderType>
    void CShader<eShaderType>::ReleaseShader()
    {
      // Remove shader
      bool bOk(false);
      {
        bOk = global::dx11::SafeRelease(m_pInternalPtr);
      }
      assert(bOk);
    }

    template<EShaderType eShaderType>
    void CShader<eShaderType>::PushShader()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx11::s_pDeviceContext;
      assert(pDeviceCtx);
      // Push Shader
      switch (eShaderType)
      {
        case EShaderType::VERTEX_SHADER:  { pDeviceCtx->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(m_pInternalPtr), nullptr, 0); } break;
        case EShaderType::PIXEL_SHADER:   { pDeviceCtx->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(m_pInternalPtr), nullptr, 0); } break;
        case EShaderType::COMPUTE_SHADER: { pDeviceCtx->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(m_pInternalPtr), nullptr, 0); } break;
        default: break;
      }
    }

    template<EShaderType eShaderType>
    void CShader<eShaderType>::DetachShader()
    {
      // Get device ctx
      ID3D11DeviceContext* pDeviceCtx = global::dx11::s_pDeviceContext;
      assert(pDeviceCtx);
      // Push Shader
      switch (eShaderType)
      {
        case EShaderType::VERTEX_SHADER:  { pDeviceCtx->VSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::PIXEL_SHADER:   { pDeviceCtx->PSSetShader(nullptr, nullptr, 0); } break;
        case EShaderType::COMPUTE_SHADER: { pDeviceCtx->CSSetShader(nullptr, nullptr, 0); } break;
        default: break;
      }
    }

    template<EShaderType eShaderType>
    CShader<eShaderType>::CShader(const BYTE* _pData, size_t _tSize)
    {
      ID3D11Device* _pDevice = global::dx11::s_pDevice;
      assert(_pDevice);

      HRESULT hResult = S_OK;
      switch (eShaderType)
      {
      case EShaderType::VERTEX_SHADER:
      {
        ID3D11VertexShader** pVertexShader = reinterpret_cast<ID3D11VertexShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateVertexShader(_pData, _tSize, nullptr, pVertexShader);
      }
      break;
      case EShaderType::PIXEL_SHADER:
      {
        ID3D11PixelShader** pPixelShader = reinterpret_cast<ID3D11PixelShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreatePixelShader(_pData, _tSize, nullptr, pPixelShader);
      }
      break;
      case EShaderType::COMPUTE_SHADER:
      {
        ID3D11ComputeShader** pComputeShader = reinterpret_cast<ID3D11ComputeShader**>(&m_pInternalPtr);
        hResult = _pDevice->CreateComputeShader(_pData, _tSize, nullptr, pComputeShader);
      }
      break;
      }
#ifdef _DEBUG
      assert(SUCCEEDED(hResult));
#endif
    }
  }
}
