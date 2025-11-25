#include "RenderTarget.h"
#include "Engine/Global/GlobalResources.h"
#include "Resources/Texture2D.h"
#include <iostream>

namespace render
{
  namespace internal_RT
  {
    static const uint32_t s_uFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  }
  // ------------------------------------
  HRESULT CRenderTarget::CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eFormat)
  {
    // Flush
    CleanRT();

    // Create texture
    m_pRTTexture = new texture::CTexture2D<render::EView::RENDER_TARGET>();

    // Set texture config
    D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
    oTextureDesc.Width = _uWidth;
    oTextureDesc.Height = _uHeight;
    oTextureDesc.MipLevels = 1;
    oTextureDesc.ArraySize = 1;
    oTextureDesc.SampleDesc.Count = 1;
    oTextureDesc.Format = _eFormat;
    oTextureDesc.BindFlags = internal_RT::s_uFlags;

    HRESULT hResult = m_pRTTexture->CreateTexture(oTextureDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating texture!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_RENDER_TARGET_VIEW_DESC oRenderTargetDesc = D3D11_RENDER_TARGET_VIEW_DESC();
    oRenderTargetDesc.Format = _eFormat;
    oRenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    oRenderTargetDesc.Texture2D.MipSlice = 0;
    hResult = m_pRTTexture->CreateView(oRenderTargetDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating target view!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_SHADER_RESOURCE_VIEW_DESC oSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    oSRVDesc.Format = _eFormat;
    oSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    oSRVDesc.Texture2D.MipLevels = 1;
    return global::dx::s_pDevice->CreateShaderResourceView(m_pRTTexture->GetData(), &oSRVDesc, &m_pSRV);
  }
  // ------------------------------------
  void CRenderTarget::ClearRT(const float _v4ClearColor[4])
  {
    global::dx::s_pDeviceContext->ClearRenderTargetView(GetRT(), _v4ClearColor);
  }
  // ------------------------------------
  void CRenderTarget::CleanRT()
  {
    global::ReleaseObject(m_pRTTexture);
    global::dx::SafeRelease(m_pSRV);
  }
}

