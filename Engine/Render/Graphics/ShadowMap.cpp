#include "ShadowMap.h"

namespace render
{
  namespace gfx
  {
    // ------------------------------------
    CShadowMap::CShadowMap(CShadowMap&& _rOther) noexcept :
      m_oDepthStencil(std::move(_rOther.m_oDepthStencil))
      , m_oShaderResource(std::move(_rOther.m_oShaderResource))
    {
    }
    // ------------------------------------
    render::gfx::CShadowMap& CShadowMap::operator=(CShadowMap&& _rOther) noexcept
    {
      if (this != &_rOther)
      {
        Release();
        m_oDepthStencil = std::move(_rOther.m_oDepthStencil);
        m_oShaderResource = std::move(_rOther.m_oShaderResource);
      }
      return *this;
    }
    // ------------------------------------
    HRESULT CShadowMap::Setup(const CRenderDeviceDX11& _rRenderDevice, uint32_t _uWidth, uint32_t _uHeight)
    {
      D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
      rTextureDesc.Width = _uWidth;
      rTextureDesc.Height = _uHeight;
      rTextureDesc.MipLevels = 1;
      rTextureDesc.ArraySize = 1;
      rTextureDesc.SampleDesc.Count = 1;
      rTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Format
      rTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Depth stencil

      // Create shadow depth texture
      texture::TTextureData rTextureCfg = texture::TTextureData();
      rTextureCfg.Descriptor = rTextureDesc;
      HRESULT hResult = m_oDepthStencil.CreateTexture(_rRenderDevice, rTextureCfg);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error creating depth stencil texture!");
        return hResult;
      }

      // Set depth stencil view desc
      D3D11_DEPTH_STENCIL_VIEW_DESC rDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
      rDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
      rDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

      // Create the depth stencil view
      hResult = m_oDepthStencil.CreateView(_rRenderDevice, rDepthStencilViewDesc);
      if (FAILED(hResult))
      {
        ERROR_LOG("Error creating stencil view!");
        return hResult;
      }

      // Creating view from texture
      D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
      rSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
      rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      rSRVDesc.Texture2D.MipLevels = 1;
      return m_oShaderResource.CreateViewFromTexture(_rRenderDevice, m_oDepthStencil, rSRVDesc);
    }
    // ------------------------------------
    void CShadowMap::Release()
    {
      m_oDepthStencil.Release();
      m_oShaderResource.Release();
    }
  }
}