#pragma once
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/RenderContext/RenderCommandsDX11.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"
#include "Libs/Macros/GlobalMacros.h"
#include <cassert>

namespace render
{
  namespace shader
  {
    template<EShader T>
    class CShader
    {
    public:
      CShader() = default;
      ~CShader() { Release(); }

      CShader(CShader&& _rOther) noexcept;
      CShader& operator=(CShader&& _rOther) noexcept;
      CShader(const CShader& _rOther) = delete;
      CShader& operator=(const CShader& _rOther) = delete;

      HRESULT Init(const CRenderDeviceDX11& _rRenderDevice, const unsigned char* _pBuffer, size_t _tSize);
      void Release();

      void Attach(const CRenderCommandsDX11& _rCommands);
      void Detach(const CRenderCommandsDX11& _rCommands);

      // Get shader
      inline auto* GetShader() const
      {
        if constexpr (T == render::EShader::E_VERTEX)
        {
          return static_cast<ID3D11VertexShader*>(m_pInternalPtr);
        }
        else if constexpr (T == render::EShader::E_HULL)
        {
          return static_cast<ID3D11HullShader*>(m_pInternalPtr);
        }
        else if constexpr (T == render::EShader::E_DOMAIN)
        {
          return static_cast<ID3D11DomainShader*>(m_pInternalPtr);
        }
        else if constexpr (T == render::EShader::E_GEOMETRY)
        {
          return static_cast<ID3D11GeometryShader*>(m_pInternalPtr);
        }
        else if constexpr (T == render::EShader::E_PIXEL)
        {
          return static_cast<ID3D11PixelShader*>(m_pInternalPtr);
        }
        else if constexpr (T == render::EShader::E_COMPUTE)
        {
          return static_cast<ID3D11ComputeShader*>(m_pInternalPtr);
        }
        else
        {
          return nullptr;
        }
      }
      inline auto* operator->() const { return GetShader(); }
      inline bool IsValid() const { return m_pInternalPtr != nullptr; }

    private:
      IUnknown* m_pInternalPtr = nullptr;
    };

    template<EShader T>
    render::shader::CShader<T>::CShader(render::shader::CShader<T>&& _rOther) noexcept :
      m_pInternalPtr(std::exchange(_rOther.m_pInternalPtr, nullptr)) {}

    template<EShader T>
    render::shader::CShader<T>& render::shader::CShader<T>::operator=(render::shader::CShader<T>&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Release();
        m_pInternalPtr = std::exchange(_rOther.m_pInternalPtr, nullptr);
      }
      return *this;
    }

    template<EShader T>
    HRESULT render::shader::CShader<T>::Init(const CRenderDeviceDX11& _rRenderDevice, const unsigned char* _pBuffer, size_t _tSize)
    {
      switch (T)
      {
        case render::EShader::E_VERTEX:
        {
          ID3D11VertexShader** pShader = reinterpret_cast<ID3D11VertexShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreateVertexShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_HULL:
        {
          ID3D11HullShader** pShader = reinterpret_cast<ID3D11HullShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreateHullShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_DOMAIN:
        {
          ID3D11DomainShader** pShader = reinterpret_cast<ID3D11DomainShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreateDomainShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_GEOMETRY:
        {
          ID3D11GeometryShader** pShader = reinterpret_cast<ID3D11GeometryShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreateGeometryShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_PIXEL:
        {
          ID3D11PixelShader** pShader = reinterpret_cast<ID3D11PixelShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreatePixelShader(_pBuffer, _tSize, nullptr, pShader);
        }
        case render::EShader::E_COMPUTE:
        {
          ID3D11ComputeShader** pShader = reinterpret_cast<ID3D11ComputeShader**>(&m_pInternalPtr);
          return _rRenderDevice->CreateComputeShader(_pBuffer, _tSize, nullptr, pShader);
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
    void CShader<T>::Attach(const CRenderCommandsDX11& _rCommands)
    {
      // Attach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { _rCommands->VSSetShader(reinterpret_cast<ID3D11VertexShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_HULL:     { _rCommands->HSSetShader(reinterpret_cast<ID3D11HullShader*>(m_pInternalPtr),     nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { _rCommands->DSSetShader(reinterpret_cast<ID3D11DomainShader*>(m_pInternalPtr),   nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { _rCommands->GSSetShader(reinterpret_cast<ID3D11GeometryShader*>(m_pInternalPtr), nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { _rCommands->PSSetShader(reinterpret_cast<ID3D11PixelShader*>(m_pInternalPtr),    nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { _rCommands->CSSetShader(reinterpret_cast<ID3D11ComputeShader*>(m_pInternalPtr),  nullptr, 0); } break;
        default: break;
      }
    }

    template<EShader T>
    void CShader<T>::Detach(const CRenderCommandsDX11& _rCommands)
    {
      // Detach shader
      switch (T)
      {
        case render::EShader::E_VERTEX:   { _rCommands->VSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_HULL:     { _rCommands->HSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_DOMAIN:   { _rCommands->DSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_GEOMETRY: { _rCommands->GSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_PIXEL:    { _rCommands->PSSetShader(nullptr, nullptr, 0); } break;
        case render::EShader::E_COMPUTE:  { _rCommands->CSSetShader(nullptr, nullptr, 0); } break;
        default: break;
      }
    }
  }
}
