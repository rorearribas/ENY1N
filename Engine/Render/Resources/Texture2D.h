#pragma once
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/RenderContext/RenderCommandsDX11.h"
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
    static constexpr uint32_t s_uRGB = 3u;
    static constexpr uint32_t s_uRGBA = 4u;

    struct TTextureData
    {
      D3D11_TEXTURE2D_DESC Descriptor = D3D11_TEXTURE2D_DESC();
      uint32_t Channels = s_uRGBA;
      void* Data = nullptr;
    };

    template<render::EView T = render::EView::UNKNOWN>
    class CTexture2D
    {
    public:
      CTexture2D() = default;
      ~CTexture2D() { Release(); }

      CTexture2D(CTexture2D&& _rOther) noexcept;
      CTexture2D& operator=(CTexture2D&& _rOther) noexcept;
      CTexture2D(const CTexture2D&) = delete;
      CTexture2D& operator=(const CTexture2D&) = delete;

      // Handler
      HRESULT CreateTexture(const CRenderDeviceDX11& _rRenderDevice, const TTextureData& _rTextureData);

      void CopyTexture(const CRenderCommandsDX11& _rCommands, ID3D11Texture2D* const _pTexture) const;
      void GetTextureSize(uint32_t& _uWidth_, uint32_t& _uHeight_) const;
      void Release();

      // Create resource
      template<typename _T>
      inline HRESULT CreateView(const CRenderDeviceDX11& _rRenderDevice, const _T& _rViewCfg)
      {
        if constexpr (T == render::EView::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(_rRenderDevice, m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(_rRenderDevice, m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(_rRenderDevice, m_pInternalTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(_rRenderDevice, m_pInternalTexture, _rViewCfg);
        }
        else
        {
          return E_FAIL;
        }
      }

      // Create resource from texture
      template<typename _T>
      inline HRESULT CreateViewFromTexture(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const _T& _rViewCfg)
      {
        if constexpr (T == render::EView::DEPTH_STENCIL && std::is_same<_T, D3D11_DEPTH_STENCIL_VIEW_DESC>::value)
        {
          return CreateDepthStencilView(_rRenderDevice, _pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::RENDER_TARGET && std::is_same<_T, D3D11_RENDER_TARGET_VIEW_DESC>::value)
        {
          return CreateRenderTargetView(_rRenderDevice, _pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::SHADER_RESOURCE && std::is_same<_T, D3D11_SHADER_RESOURCE_VIEW_DESC>::value)
        {
          return CreateShaderResourceView(_rRenderDevice, _pTargetTexture, _rViewCfg);
        }
        else if constexpr (T == render::EView::UNORDERED_ACCESS && std::is_same<_T, D3D11_UNORDERED_ACCESS_VIEW_DESC>::value)
        {
          return CreateUnorderedAccessView(_rRenderDevice, _pTargetTexture, _rViewCfg);
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
      HRESULT CreateDepthStencilView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC& _rViewDesc);
      HRESULT CreateRenderTargetView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC& _rViewDesc);
      HRESULT CreateShaderResourceView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc);
      HRESULT CreateUnorderedAccessView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_UNORDERED_ACCESS_VIEW_DESC& _rViewDesc);

    private:
      ID3D11Texture2D* m_pInternalTexture = nullptr;
      ID3D11View* m_pInternalView = nullptr;
    };

    template<render::EView T>
    render::texture::CTexture2D<T>::CTexture2D(render::texture::CTexture2D<T>&& _rOther) noexcept
      : m_pInternalTexture(std::exchange(_rOther.m_pInternalTexture, nullptr))
      , m_pInternalView(std::exchange(_rOther.m_pInternalView, nullptr))
    {
    }

    template<render::EView T>
    render::texture::CTexture2D<T>& render::texture::CTexture2D<T>::operator=(render::texture::CTexture2D<T>&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Release();
        m_pInternalTexture = std::exchange(_rOther.m_pInternalTexture, nullptr);
        m_pInternalView = std::exchange(_rOther.m_pInternalView, nullptr);
      }
      return *this;
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateTexture(const CRenderDeviceDX11& _rRenderDevice, const TTextureData& _rTextureData)
    {
      // Clear
      global::api::SafeRelease(m_pInternalTexture);

      // Create texture from data
      D3D11_SUBRESOURCE_DATA rSubresourceData = D3D11_SUBRESOURCE_DATA();
      rSubresourceData.SysMemPitch = _rTextureData.Descriptor.Width * _rTextureData.Channels;
      rSubresourceData.pSysMem = _rTextureData.Data;

      // Create texture
      return _rRenderDevice->CreateTexture2D(&_rTextureData.Descriptor, _rTextureData.Data ? &rSubresourceData : nullptr, &m_pInternalTexture
      );
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::GetTextureSize(uint32_t& _uWidth_, uint32_t& _uHeight_) const
    {
      // Fail-safe initialization to prevent garbage values
      _uWidth_ = 0;
      _uHeight_ = 0;

      if (m_pInternalTexture)
      {
        // Get texture info
        D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
        m_pInternalTexture->GetDesc(&rTextureDesc);

        // Set size
        _uWidth_ = rTextureDesc.Width;
        _uHeight_ = rTextureDesc.Height;
      }
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::CopyTexture(const CRenderCommandsDX11& _rCommands, ID3D11Texture2D* const _pTexture) const
    {
#ifdef _DEBUG
      assert(m_pInternalTexture && _pTexture);
#endif // DEBUG
      _rCommands->CopyResource(m_pInternalTexture, _pTexture);
    }

    template<render::EView T>
    void render::texture::CTexture2D<T>::Release()
    {
      global::api::SafeRelease(m_pInternalTexture);
      global::api::SafeRelease(m_pInternalView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateDepthStencilView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::DEPTH_STENCIL);
#endif // DEBUG
      global::api::SafeRelease(m_pInternalView);
      ID3D11DepthStencilView** pView = reinterpret_cast<ID3D11DepthStencilView**>(&m_pInternalView);
      return _rRenderDevice->CreateDepthStencilView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateRenderTargetView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_RENDER_TARGET_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::RENDER_TARGET);
#endif // DEBUG
      global::api::SafeRelease(m_pInternalView);
      ID3D11RenderTargetView** pView = reinterpret_cast<ID3D11RenderTargetView**>(&m_pInternalView);
      return _rRenderDevice->CreateRenderTargetView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateShaderResourceView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::SHADER_RESOURCE);
#endif // DEBUG
      global::api::SafeRelease(m_pInternalView);
      ID3D11ShaderResourceView** pView = reinterpret_cast<ID3D11ShaderResourceView**>(&m_pInternalView);
      return _rRenderDevice->CreateShaderResourceView(_pTargetTexture, &_rViewDesc, pView);
    }

    template<render::EView T>
    HRESULT render::texture::CTexture2D<T>::CreateUnorderedAccessView(const CRenderDeviceDX11& _rRenderDevice, ID3D11Texture2D* const _pTargetTexture, const D3D11_UNORDERED_ACCESS_VIEW_DESC& _rViewDesc)
    {
#ifdef _DEBUG
      assert(T == EView::UNORDERED_ACCESS);
#endif // DEBUG
      global::api::SafeRelease(m_pInternalView);
      ID3D11UnorderedAccessView** pView = reinterpret_cast<ID3D11UnorderedAccessView**>(&m_pInternalView);
      return _rRenderDevice->CreateUnorderedAccessView(_pTargetTexture, &_rViewDesc, pView);
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


