#pragma once
#include "Engine/Global/GlobalResources.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Shaders/Shader.h"

#include <string>
#include <cassert>
#include <type_traits>

namespace render
{
  namespace texture
  {
    template<render::EView T = render::EView::UNKNOWN>
    class CTexture2D
    {
    public:
      static constexpr uint32_t s_uChannels = 4;

    public:
      CTexture2D() = default;
      ~CTexture2D() { Clear(); }

      void CopyTexture(ID3D11Texture2D* _pTexture);
      void AttachTexture(uint32_t _uSlot, EShader _eShaderType);

      // Texture and sampler
      HRESULT CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _oTextureCfg, uint32_t _uChannels = 4);
      HRESULT CreateTexture(const D3D11_TEXTURE2D_DESC& _oTextureCfg);

      inline ID3D11Texture2D* const GetData() { return m_pInternalTexture; }
      inline void SetTexture(ID3D11Texture2D* _pTexture) { m_pInternalTexture = _pTexture; }

      // Create resource
      template<typename _T>
      inline HRESULT CreateView(const _T& _oViewCfg)
      {
        if constexpr (T == render::EView::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(_oViewCfg);
        }
        else if constexpr (T == render::EView::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(_oViewCfg);
        }
        else if constexpr (T == render::EView::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(_oViewCfg);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(_oViewCfg);
        }
        else
        {
          return E_FAIL;
        }
      }

      // Get resource
      inline auto* GetView() const
      {
        if constexpr (T == render::EView::SHADER_RESOURCE)
        {
          return static_cast<ID3D11ShaderResourceView*>(m_pInternalView);
        }
        else if constexpr (T == render::EView::RENDER_TARGET)
        {
          return static_cast<ID3D11RenderTargetView*>(m_pInternalView);
        }
        else if constexpr (T == render::EView::DEPTH_STENCIL)
        {
          return static_cast<ID3D11DepthStencilView*>(m_pInternalView);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS)
        {
          return static_cast<ID3D11UnorderedAccessView*>(m_pInternalView);
        }
        else
        {
          return nullptr;
        }
      }

      // Override operators
      inline operator ID3D11Texture2D* () const { return *m_pInternalTexture; }
      inline operator const ID3D11Texture2D* () const { return *m_pInternalTexture; }

    private:
      void Clear();

      void ReleaseTexture();
      void ReleaseView();

      // View creation
      HRESULT CreateDepthStencilView(const D3D11_DEPTH_STENCIL_VIEW_DESC& _oViewDesc);
      HRESULT CreateRenderTargetView(const D3D11_RENDER_TARGET_VIEW_DESC& _oViewDesc);
      HRESULT CreateShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& _oViewDesc);
      HRESULT CreateUnorderedAccessView(const D3D11_UNORDERED_ACCESS_VIEW_DESC& _oViewDesc);

    private:
      ID3D11Texture2D* m_pInternalTexture = nullptr;
      ID3D11View* m_pInternalView = nullptr;
    };

    template<render::EView T>
    void render::texture::CTexture2D<T>::CopyTexture(ID3D11Texture2D* _pTexture)
    {
#ifdef _DEBUG
      assert(m_pInternalTexture && _pTexture);
#endif // DEBUG
      global::dx::s_pDeviceContext->CopyResource(m_pInternalTexture, _pTexture);
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::AttachTexture(uint32_t _uSlot, EShader _eShaderType)
    {
      if constexpr (T != render::EShader::SHADER_RESOURCE)
      {
        return;
      }

      // Attach texture to current pipeline
      ID3D11ShaderResourceView* pShaderResource = GetView();
      if (pShaderResource)
      {
        switch (_eShaderType)
        {
          case render::EShader::E_VERTEX:   global::dx::s_pDeviceContext->VSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case render::EShader::E_HULL:     global::dx::s_pDeviceContext->HSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case render::EShader::E_DOMAIN:   global::dx::s_pDeviceContext->DSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case render::EShader::E_GEOMETRY: global::dx::s_pDeviceContext->GSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case render::EShader::E_PIXEL:    global::dx::s_pDeviceContext->PSSetShaderResources(_uSlot, 1, &pShaderResource); break;
          case render::EShader::E_COMPUTE:  global::dx::s_pDeviceContext->CSSetShaderResources(_uSlot, 1, &pShaderResource); break;
        }
      }
    }

    template<render::EView T>
    void texture::CTexture2D<T>::Clear()
    {
      ReleaseTexture();
      ReleaseView();
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::ReleaseTexture()
    {
      global::dx::SafeRelease(m_pInternalTexture);
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::ReleaseView()
    {
      global::dx::SafeRelease(m_pInternalView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _oTextureDesc, uint32_t _uChannels)
    {
      // Clear
      ReleaseTexture();

      // Create texture using raw data!
      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _pData;
      oSubresourceData.SysMemPitch = _oTextureDesc.Width * _uChannels;

      // Create texture
      return global::dx::s_pDevice->CreateTexture2D(&_oTextureDesc, &oSubresourceData, &m_pInternalTexture);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(const D3D11_TEXTURE2D_DESC& _oTextureDesc)
    {
      // Clear
      ReleaseTexture();

      // Create empty texture!
      return global::dx::s_pDevice->CreateTexture2D(&_oTextureDesc, nullptr, &m_pInternalTexture);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateDepthStencilView(const D3D11_DEPTH_STENCIL_VIEW_DESC& _oViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::DEPTH_STENCIL);
#endif // DEBUG
      ReleaseView();
      ID3D11DepthStencilView** pView = reinterpret_cast<ID3D11DepthStencilView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateDepthStencilView(m_pInternalTexture, &_oViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateRenderTargetView(const D3D11_RENDER_TARGET_VIEW_DESC& _oViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::RENDER_TARGET);
#endif // DEBUG
      ReleaseView();
      ID3D11RenderTargetView** pView = reinterpret_cast<ID3D11RenderTargetView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateRenderTargetView(m_pInternalTexture, &_oViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& _oViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::SHADER_RESOURCE);
#endif // DEBUG
      ReleaseView();
      ID3D11ShaderResourceView** pView = reinterpret_cast<ID3D11ShaderResourceView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateShaderResourceView(m_pInternalTexture, &_oViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateUnorderedAccessView(const D3D11_UNORDERED_ACCESS_VIEW_DESC& _oDesc)
    {
#ifdef _DEBUG
      assert(T == EView::UNORDERED_ACCESS);
#endif // DEBUG
      ReleaseView();
      ID3D11UnorderedAccessView** pView = reinterpret_cast<ID3D11UnorderedAccessView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateUnorderedAccessView(m_pInternalTexture, &_oDesc, pView);
    }

    // Typedefs
    typedef texture::CTexture2D<render::EView::DEPTH_STENCIL> TDepthStencil2D;
    typedef texture::CTexture2D<render::EView::RENDER_TARGET> TRenderTarget2D;
    typedef texture::CTexture2D<render::EView::SHADER_RESOURCE> TShaderResource2D;
    typedef texture::CTexture2D<render::EView::UNORDERED_ACCESS> TUnorderedAccess2D;

    // Shared
    typedef std::shared_ptr<TShaderResource2D> TSharedTexture;
  }
}


