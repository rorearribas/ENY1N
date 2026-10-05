#include "RenderTarget.h"
#include "Engine/Global/GlobalResources.h"
#include <iostream>

namespace render
{
  namespace internal_RT
  {
    static const uint32_t s_uFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  }
  // ------------------------------------
  HRESULT CRenderTarget::Init(const CRenderDeviceDX11& _rRenderDevice, const TRenderTargetDesc& _rDesc)
  {
    // Flush
    Release();

    // Set texture config
    D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
    rTextureDesc.Width = _rDesc.uWidth;
    rTextureDesc.Height = _rDesc.uHeight;
    rTextureDesc.MipLevels = 1;
    rTextureDesc.ArraySize = 1;
    rTextureDesc.SampleDesc.Count = 1;
    rTextureDesc.Format = _rDesc.eFormat;
    rTextureDesc.BindFlags = internal_RT::s_uFlags;

    texture::TTextureDesc rTextureCfg = texture::TTextureDesc();
    rTextureCfg.Descriptor = rTextureDesc;
    HRESULT hResult = m_oRTTexture.CreateTexture(_rRenderDevice, rTextureCfg);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating texture!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_RENDER_TARGET_VIEW_DESC rRenderTargetDesc = D3D11_RENDER_TARGET_VIEW_DESC();
    rRenderTargetDesc.Format = _rDesc.eFormat;
    rRenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rRenderTargetDesc.Texture2D.MipSlice = 0;
    hResult = m_oRTTexture.CreateView(_rRenderDevice, rRenderTargetDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating target view!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    rSRVDesc.Format = _rDesc.eFormat;
    rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rSRVDesc.Texture2D.MipLevels = 1;
    return _rRenderDevice->CreateShaderResourceView(m_oRTTexture, &rSRVDesc, &m_pShaderResourceView);
  }
  // ------------------------------------
  void CRenderTarget::SetClearColor(const CRenderCommandsDX11& _rCommands, const float _v4ClearColor[4])
  {
    if (ID3D11RenderTargetView* pRenderTargetView = GetRenderTargetView())
    {
      _rCommands->ClearRenderTargetView(pRenderTargetView, _v4ClearColor);
    }
  }
  // ------------------------------------
  void CRenderTarget::Release()
  {
    global::api::SafeRelease(m_pShaderResourceView);
    m_oRTTexture.Release();
  }
}

