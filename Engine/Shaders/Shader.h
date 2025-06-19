#pragma once
#include <d3d11.h>
#include <cassert>

namespace render
{
  namespace shader
  {
    enum EShaderType
    {
      PIXEL_SHADER,
      VERTEX_SHADER,
      COMPUTE_SHADER
    };

    template<EShaderType eShaderType>
    class CShader
    {
    public:
      CShader(const BYTE* _pData, size_t _tSize);
      ~CShader() { ReleaseShader(); }

      void PushShader();
      void ReleaseShader();

    private:
      void* m_pShaderData = nullptr;
    };

    template<EShaderType eShaderType>
    void render::shader::CShader<eShaderType>::ReleaseShader()
    {
      switch (eShaderType)
      {
        case render::shader::VERTEX_SHADER: 
        {
          ID3D11VertexShader* pVertexShader = static_cast<ID3D11VertexShader*>(m_pShaderData);
          global::dx11::SafeRelease(pVertexShader);
        }
        break;
        case render::shader::PIXEL_SHADER: 
        {
          ID3D11PixelShader* pPixelShader = static_cast<ID3D11PixelShader*>(m_pShaderData);
          global::dx11::SafeRelease(pPixelShader);
        }
        break;
        case render::shader::COMPUTE_SHADER: 
        {
          ID3D11ComputeShader* pComputeShader = static_cast<ID3D11ComputeShader*>(m_pShaderData);
          global::dx11::SafeRelease(pComputeShader);
        }
        break;
      }

      // Set as nullptr
      m_pShaderData = nullptr;
    }

    template<EShaderType eShaderType>
    void render::shader::CShader<eShaderType>::PushShader()
    {
      switch (eShaderType)
      {
      case render::shader::VERTEX_SHADER:
        global::dx11::s_pDeviceContext->VSSetShader(static_cast<ID3D11VertexShader*>(m_pShaderData), nullptr, 0);
        break;
      case render::shader::PIXEL_SHADER:
        global::dx11::s_pDeviceContext->PSSetShader(static_cast<ID3D11PixelShader*>(m_pShaderData), nullptr, 0);
        break;
      case render::shader::COMPUTE_SHADER:
        global::dx11::s_pDeviceContext->CSSetShader(static_cast<ID3D11ComputeShader*>(m_pShaderData), nullptr, 0);
        break;
      default:
        break;
      }
    }

    template<EShaderType eShaderType>
    render::shader::CShader<eShaderType>::CShader(const BYTE* _pData, size_t _tSize)
    {
      HRESULT hResult = S_OK;
      switch (eShaderType)
      {
      case render::shader::PIXEL_SHADER:
      {
        ID3D11PixelShader* pPixelShader = nullptr;
        hResult = global::dx11::s_pDevice->CreatePixelShader(_pData, _tSize, nullptr, &pPixelShader);
#ifdef _DEBUG
        assert(!FAILED(hResult));
#endif
        m_pShaderData = pPixelShader;
      }
      break;
      case render::shader::VERTEX_SHADER:
      {
        ID3D11VertexShader* pVertexShader = nullptr;
        hResult = global::dx11::s_pDevice->CreateVertexShader(_pData, _tSize, nullptr, &pVertexShader);
#ifdef _DEBUG
        assert(!FAILED(hResult));
#endif
        m_pShaderData = pVertexShader;
      }
      break;
      case render::shader::COMPUTE_SHADER:
      {
        ID3D11ComputeShader* pComputeShader = nullptr;
        hResult = global::dx11::s_pDevice->CreateComputeShader(_pData, _tSize, nullptr, &pComputeShader);
#ifdef _DEBUG
        assert(!FAILED(hResult));
#endif
        m_pShaderData = pComputeShader;
      }
      break;
      }
    }
  }
}
