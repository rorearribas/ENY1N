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
    D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
    rTextureDesc.Width = _uWidth;
    rTextureDesc.Height = _uHeight;
    rTextureDesc.MipLevels = 1;
    rTextureDesc.ArraySize = 1;
    rTextureDesc.SampleDesc.Count = 1;
    rTextureDesc.Format = _eFormat;
    rTextureDesc.BindFlags = internal_RT::s_uFlags;

    HRESULT hResult = m_pRTTexture->CreateTexture(rTextureDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating texture!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_RENDER_TARGET_VIEW_DESC rRenderTargetDesc = D3D11_RENDER_TARGET_VIEW_DESC();
    rRenderTargetDesc.Format = _eFormat;
    rRenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rRenderTargetDesc.Texture2D.MipSlice = 0;
    hResult = m_pRTTexture->CreateView(rRenderTargetDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating target view!");
      return hResult;
    }

    // Creating a view of the texture to be used when binding it as a render target
    D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    rSRVDesc.Format = _eFormat;
    rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rSRVDesc.Texture2D.MipLevels = 1;
    return global::dx::s_pDevice->CreateShaderResourceView(m_pRTTexture->GetData(), &rSRVDesc, &m_pSRV);
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

