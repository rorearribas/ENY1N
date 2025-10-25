#pragma once
#include <d3d11.h>
#include <string>
#include <cassert>
#include <type_traits>
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Shaders/Shader.h"

namespace render
{
  namespace texture
  {
    template<EViewType T = EViewType::UNKNOWN>
    class CTexture2D
    {
    public:
      static constexpr uint32_t s_uChannels = 4;

    public:
      CTexture2D() = default;
      ~CTexture2D() { ReleaseTexture(); ReleaseSampler(); ReleaseView(); }

      void CopyTexture(ID3D11Texture2D* _pTexture);
      void AttachTexture(uint32_t _uSlot, EShaderType _eShaderType);
      void AttachSampler(uint32_t _uSlot, EShaderType _eShaderType);

      // Texture and sampler
      HRESULT CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _oTextureCfg, uint32_t _uChannels = 4);
      HRESULT CreateTexture(const D3D11_TEXTURE2D_DESC& _oTextureCfg);
      HRESULT CreateSampler(const D3D11_SAMPLER_DESC& _oSamplerCfg);

      inline ID3D11Texture2D* const GetTexture() { return m_pInternalTexture; }
      inline void SetTexture(ID3D11Texture2D* _pTexture) { m_pInternalTexture = _pTexture; }
      inline ID3D11SamplerState* const GetSampler() { return m_pSamplerState; }
      inline void SetSampler(ID3D11SamplerState* _pSampler) { m_pSamplerState = _pSampler; }

      // Create resource
      template<typename _T>
      inline HRESULT CreateView(const _T& _oCfg)
      {
        if constexpr (T == EViewType::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(_oCfg);
        }
        else if constexpr (T == EViewType::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(_oCfg);
        }
        else if constexpr (T == EViewType::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(_oCfg);
        }
        else if constexpr (T == EViewType::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(_oCfg);
        }
        else
        {
          return E_FAIL;
        }
      }
      // Get resource
      inline auto* GetView() const
      {
        if constexpr (T == EViewType::SHADER_RESOURCE)
        {
          return static_cast<ID3D11ShaderResourceView*>(m_pInternalView);
        }
        else if constexpr (T == EViewType::RENDER_TARGET)
        {
          return static_cast<ID3D11RenderTargetView*>(m_pInternalView);
        }
        else if constexpr (T == EViewType::DEPTH_STENCIL)
        {
          return static_cast<ID3D11DepthStencilView*>(m_pInternalView);
        }
        else if constexpr (T == EViewType::UNORDERED_ACCESS)
        {
          return static_cast<ID3D11UnorderedAccessView*>(m_pInternalView);
        }
        else
        {
          return nullptr;
        }
      }

      // Override operators
      inline operator ID3D11Texture2D* () const { return m_pInternalTexture; }
      inline operator const ID3D11Texture2D* () const { return m_pInternalTexture; }

    private:
      void ReleaseTexture();
      void ReleaseSampler();
      void ReleaseView();

      // View creation
      HRESULT CreateDepthStencilView(const D3D11_DEPTH_STENCIL_VIEW_DESC& _oDesc);
      HRESULT CreateRenderTargetView(const D3D11_RENDER_TARGET_VIEW_DESC& _oDesc);
      HRESULT CreateShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& _oDesc);
      HRESULT CreateUnorderedAccessView(const D3D11_UNORDERED_ACCESS_VIEW_DESC& _oDesc);

    private:
      // DirectX
      ID3D11View* m_pInternalView = nullptr;
      ID3D11Texture2D* m_pInternalTexture = nullptr;
      ID3D11SamplerState* m_pSamplerState = nullptr;
    };

    template<EViewType T>
    void render::texture::CTexture2D<T>::CopyTexture(ID3D11Texture2D* _pTexture)
    {
      assert(m_pInternalTexture && _pTexture);
      global::dx::s_pDeviceContext->CopyResource(m_pInternalTexture, _pTexture);
    }

    template<EViewType T>
    void render::texture::CTexture2D<T>::AttachTexture(uint32_t _uSlot, EShaderType _eShaderType)
    {
      if constexpr (T != SHADER_RESOURCE)
      {
        return;
      }

      // Attach texture to current pipeline
      ID3D11ShaderResourceView* pShaderResource = GetView();
      if (pShaderResource)
      {
        switch (_eShaderType)
        {
          case E_VERTEX:   global::dx::s_pDeviceContext->VSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case E_HULL:     global::dx::s_pDeviceContext->HSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case E_DOMAIN:   global::dx::s_pDeviceContext->DSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case E_GEOMETRY: global::dx::s_pDeviceContext->GSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case E_PIXEL:    global::dx::s_pDeviceContext->PSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case E_COMPUTE:  global::dx::s_pDeviceContext->CSSetShaderResources(_uSlot, 1, &pShaderResource); break;
        }
      }
    }

    template<EViewType T>
    void render::texture::CTexture2D<T>::AttachSampler(uint32_t _uSlot, EShaderType _eShaderType)
    {
      if constexpr (T != SHADER_RESOURCE)
      {
        return;
      }

      // Attach sampler
      if (m_pSamplerState)
      {
        switch (_eShaderType)
        {
          case E_VERTEX:   global::dx::s_pDeviceContext->VSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
          case E_HULL:     global::dx::s_pDeviceContext->HSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
          case E_DOMAIN:   global::dx::s_pDeviceContext->DSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
          case E_GEOMETRY: global::dx::s_pDeviceContext->GSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
          case E_PIXEL:    global::dx::s_pDeviceContext->PSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
          case E_COMPUTE:  global::dx::s_pDeviceContext->CSSetSamplers(_uSlot, 1, &m_pSamplerState); break;
        }
      }
    }

    template<EViewType T>
    void render::texture::CTexture2D<T>::ReleaseTexture()
    {
      global::dx::SafeRelease(m_pInternalTexture);
    }

    template<EViewType T>
    void render::texture::CTexture2D<T>::ReleaseSampler()
    {
      global::dx::SafeRelease(m_pSamplerState);
    }

    template<EViewType T>
    void render::texture::CTexture2D<T>::ReleaseView()
    {
      global::dx::SafeRelease(m_pInternalView);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateSampler(const D3D11_SAMPLER_DESC& _oSamplerCfg)
    {
      ReleaseSampler();
      return global::dx::s_pDevice->CreateSamplerState(&_oSamplerCfg, &m_pSamplerState);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _oTextureCfg, uint32_t _uChannels)
    {
      // Clear
      ReleaseTexture();

      // Create texture using raw data!
      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _pData;
      oSubresourceData.SysMemPitch = _oTextureCfg.Width * _uChannels;

      // Create texture
      return global::dx::s_pDevice->CreateTexture2D(&_oTextureCfg, &oSubresourceData, &m_pInternalTexture);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(const D3D11_TEXTURE2D_DESC& _oTextureCfg)
    {
      // Flush
      ReleaseTexture();
      // Clear empty texture!
      return global::dx::s_pDevice->CreateTexture2D(&_oTextureCfg, nullptr, &m_pInternalTexture);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateDepthStencilView(const D3D11_DEPTH_STENCIL_VIEW_DESC& _oDesc)
    {
      assert(T == EViewType::DEPTH_STENCIL);
      ReleaseView();
      ID3D11DepthStencilView** pView = reinterpret_cast<ID3D11DepthStencilView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateDepthStencilView(m_pInternalTexture, &_oDesc, pView);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateRenderTargetView(const D3D11_RENDER_TARGET_VIEW_DESC& _oDesc)
    {
      assert(T == EViewType::RENDER_TARGET);
      ReleaseView();
      ID3D11RenderTargetView** pView = reinterpret_cast<ID3D11RenderTargetView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateRenderTargetView(m_pInternalTexture, &_oDesc, pView);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& _oDesc)
    {
      assert(T == EViewType::SHADER_RESOURCE);
      ReleaseView();
      ID3D11ShaderResourceView** pView = reinterpret_cast<ID3D11ShaderResourceView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateShaderResourceView(m_pInternalTexture, &_oDesc, pView);
    }

    template<EViewType T>
    HRESULT render::texture::CTexture2D<T>::CreateUnorderedAccessView(const D3D11_UNORDERED_ACCESS_VIEW_DESC& _oDesc)
    {
      assert(T == EViewType::UNORDERED_ACCESS);
      ReleaseView();
      ID3D11UnorderedAccessView** pView = reinterpret_cast<ID3D11UnorderedAccessView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateUnorderedAccessView(m_pInternalTexture, &_oDesc, pView);
    }
  }
}


