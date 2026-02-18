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
      ~CTexture2D() { Release(); }

      void Attach(uint32_t _uSlot, EShader _eShaderType);
      void Detach(uint32_t _uSlot, EShader _eShaderType);

      void GetTextureSize(uint32_t& _uWidth_, uint32_t& _uHeight_) const;
      void CopyTexture(ID3D11Texture2D* const _pTexture) const;
      void Release();

      // Texture and sampler
      HRESULT CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _rTextureCfg, uint32_t _uChannels = 4);
      HRESULT CreateTexture(const D3D11_TEXTURE2D_DESC& _rTextureCfg);

      // Create resource
      template<typename _T>
      inline HRESULT CreateView(const _T& _rViewCfg)
      {
        if constexpr (T == render::EView::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(m_pInternalTexture, _rViewCfg);
        }
        else
        {
          return E_FAIL;
        }
      }

      // Create resource from texture
      template<typename _T>
      inline HRESULT CreateViewFromTexture(ID3D11Texture2D* const _pTargetTexture, const _T& _rViewCfg)
      {
        if constexpr (T == render::EView::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(_pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(_pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(_pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(_pTargetTexture, _rViewCfg);
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
      inline operator ID3D11Texture2D* () const { return m_pInternalTexture; }
      inline operator const ID3D11Texture2D* () const { return m_pInternalTexture; }

    private:
      // View creation
      HRESULT CreateDepthStencilView(ID3D11Texture2D* const _pTargetTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC& _rViewDesc);
      HRESULT CreateRenderTargetView(ID3D11Texture2D* const _pTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC& _rViewDesc);
      HRESULT CreateShaderResourceView(ID3D11Texture2D* const _pTargetTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc);
      HRESULT CreateUnorderedAccessView(ID3D11Texture2D* const _pTargetTexture, const D3D11_UNORDERED_ACCESS_VIEW_DESC& _rViewDesc);

    private:
      ID3D11Texture2D* m_pInternalTexture = nullptr;
      ID3D11View* m_pInternalView = nullptr;

      uint32_t m_uWidth = 0;
      uint32_t m_uHeight = 0;
    };

    template<render::EView T>
    void render::texture::CTexture2D<T>::GetTextureSize(uint32_t& _uWidth_, uint32_t& _uHeight_) const
    {
      _uWidth_ = m_uWidth;
      _uHeight_ = m_uHeight;
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::CopyTexture(ID3D11Texture2D* const _pTexture) const
    {
#ifdef _DEBUG
      assert(m_pInternalTexture && _pTexture);
#endif // DEBUG
      global::dx::s_pDeviceContext->CopyResource(m_pInternalTexture, _pTexture);
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::Release()
    {
      global::dx::SafeRelease(m_pInternalTexture);
      global::dx::SafeRelease(m_pInternalView);
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::Attach(uint32_t _uSlot, EShader _eShaderType)
    {
      if constexpr (T != render::EShader::SHADER_RESOURCE)
      {
        return;
      }

      // Attach slot
      ID3D11ShaderResourceView* pShaderView = GetView();
      if (pShaderView)
      {
        switch (_eShaderType)
        {
          case render::EShader::E_VERTEX:   global::dx::s_pDeviceContext->VSSetShaderResources(_uSlot, 1, &pShaderView); break;
          case render::EShader::E_HULL:     global::dx::s_pDeviceContext->HSSetShaderResources(_uSlot, 1, &pShaderView); break;
          case render::EShader::E_DOMAIN:   global::dx::s_pDeviceContext->DSSetShaderResources(_uSlot, 1, &pShaderView); break;
          case render::EShader::E_GEOMETRY: global::dx::s_pDeviceContext->GSSetShaderResources(_uSlot, 1, &pShaderView); break;
          case render::EShader::E_PIXEL:    global::dx::s_pDeviceContext->PSSetShaderResources(_uSlot, 1, &pShaderView); break;
          case render::EShader::E_COMPUTE:  global::dx::s_pDeviceContext->CSSetShaderResources(_uSlot, 1, &pShaderView); break;
        }
      }
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::Detach(uint32_t _uSlot, EShader _eShaderType)
    {
      if constexpr (T != render::EShader::SHADER_RESOURCE)
      {
        return;
      }

      // Detach slot
      switch (_eShaderType)
      {
        case render::EShader::E_VERTEX:   global::dx::s_pDeviceContext->VSSetShaderResources(_uSlot, 1, nullptr); break;
        case render::EShader::E_HULL:     global::dx::s_pDeviceContext->HSSetShaderResources(_uSlot, 1, nullptr); break;
        case render::EShader::E_DOMAIN:   global::dx::s_pDeviceContext->DSSetShaderResources(_uSlot, 1, nullptr); break;
        case render::EShader::E_GEOMETRY: global::dx::s_pDeviceContext->GSSetShaderResources(_uSlot, 1, nullptr); break;
        case render::EShader::E_PIXEL:    global::dx::s_pDeviceContext->PSSetShaderResources(_uSlot, 1, nullptr); break;
        case render::EShader::E_COMPUTE:  global::dx::s_pDeviceContext->CSSetShaderResources(_uSlot, 1, nullptr); break;
      }
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _rTextureDesc, uint32_t _uChannels)
    {
      // Clear
      global::dx::SafeRelease(m_pInternalTexture);

      // Create texture using raw data!
      D3D11_SUBRESOURCE_DATA rSubresourceData = D3D11_SUBRESOURCE_DATA();
      rSubresourceData.pSysMem = _pData;
      rSubresourceData.SysMemPitch = _rTextureDesc.Width * _uChannels;

      // Set size
      m_uWidth = _rTextureDesc.Width;
      m_uHeight = _rTextureDesc.Height;

      // Create texture
      return global::dx::s_pDevice->CreateTexture2D(&_rTextureDesc, &rSubresourceData, &m_pInternalTexture);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(const D3D11_TEXTURE2D_DESC& _rTextureDesc)
    {
      // Clear
      global::dx::SafeRelease(m_pInternalTexture);

      // Set size
      m_uWidth = _rTextureDesc.Width;
      m_uHeight = _rTextureDesc.Height;

      // Create empty texture!
      return global::dx::s_pDevice->CreateTexture2D(&_rTextureDesc, nullptr, &m_pInternalTexture);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateDepthStencilView(ID3D11Texture2D* const _pTargetTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::DEPTH_STENCIL);
#endif // DEBUG
      global::dx::SafeRelease(m_pInternalView);
      ID3D11DepthStencilView** pView = reinterpret_cast<ID3D11DepthStencilView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateDepthStencilView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateRenderTargetView(ID3D11Texture2D* const _pTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::RENDER_TARGET);
#endif // DEBUG
      global::dx::SafeRelease(m_pInternalView);
      ID3D11RenderTargetView** pView = reinterpret_cast<ID3D11RenderTargetView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateRenderTargetView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateShaderResourceView(ID3D11Texture2D* const _pTargetTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::SHADER_RESOURCE);
#endif // DEBUG
      global::dx::SafeRelease(m_pInternalView);
      ID3D11ShaderResourceView** pView = reinterpret_cast<ID3D11ShaderResourceView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateShaderResourceView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateUnorderedAccessView(ID3D11Texture2D* const _pTargetTexture, const D3D11_UNORDERED_ACCESS_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::UNORDERED_ACCESS);
#endif // DEBUG
      global::dx::SafeRelease(m_pInternalView);
      ID3D11UnorderedAccessView** pView = reinterpret_cast<ID3D11UnorderedAccessView**>(&m_pInternalView);
      return global::dx::s_pDevice->CreateUnorderedAccessView(_pTargetTexture, &_rViewDesc, pView);
    }

    // Typedefs
    typedef texture::CTexture2D<render::EView::DEPTH_STENCIL> TDepthStencil;
    typedef texture::CTexture2D<render::EView::RENDER_TARGET> TRenderTarget;
    typedef texture::CTexture2D<render::EView::SHADER_RESOURCE> TShaderResource;
    typedef texture::CTexture2D<render::EView::UNORDERED_ACCESS> TUnorderedAccess;

    // Shared
    typedef std::shared_ptr<TShaderResource> TSharedTexture;
  }
}


