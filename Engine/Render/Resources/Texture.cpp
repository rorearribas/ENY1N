#include "Texture.h"
#include <iostream>
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace texture
  {
    // ------------------------------------
    CTexture::~CTexture()
    {
      ClearTexture();
    }
    // ------------------------------------
    void CTexture::BindTexture()
    {
      if (m_pShaderResourceView && m_pSamplerState)
      {
        global::dx11::s_pDeviceContext->PSSetShaderResources(0, 1, &m_pShaderResourceView);
        global::dx11::s_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
      }
    }
    // ------------------------------------
    HRESULT CTexture::SetTexture(unsigned char* _pTexture, uint32_t _uWidth, uint32_t _uHeight)
    {
      // Clear
      ClearTexture();

      // Set values
      m_uTextureWidth = _uWidth;
      m_uTextureHeight = _uHeight;

      // Create texture
      D3D11_TEXTURE2D_DESC oTextureDesc = {};
      oTextureDesc.Width = m_uTextureWidth;
      oTextureDesc.Height = m_uTextureWidth;
      oTextureDesc.MipLevels = 1;
      oTextureDesc.ArraySize = 1;
      oTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      oTextureDesc.SampleDesc.Count = 1;
      oTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

      D3D11_SUBRESOURCE_DATA subresource_data = {};
      subresource_data.pSysMem = _pTexture;
      subresource_data.SysMemPitch = m_uTextureWidth * 4;

      HRESULT hr = global::dx11::s_pDevice->CreateTexture2D(&oTextureDesc, &subresource_data, &m_pTexture);
      if (FAILED(hr))
      {
        std::cout << "Failed to create texture 2D" << std::endl;
        return hr;
      }

      // Create texture view
      D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = {};
      oShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      oShaderResourceViewDesc.Texture2D.MipLevels = 1;
      hr = global::dx11::s_pDevice->CreateShaderResourceView(m_pTexture, &oShaderResourceViewDesc, &m_pShaderResourceView);
      if (FAILED(hr))
      {
        std::cout << "Failed to create render target view" << std::endl;
        return hr;
      }

      // Create sampler state
      D3D11_SAMPLER_DESC oSamplerDesc = {};
      oSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      oSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
      oSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
      oSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
      hr = global::dx11::s_pDevice->CreateSamplerState(&oSamplerDesc, &m_pSamplerState);
      if (FAILED(hr))
      {
        std::cout << "Failed to create sampler state" << std::endl;
        return hr;
      }

      return S_OK;
    }
    // ------------------------------------
    void CTexture::ClearTexture()
    {
      global::dx11::SafeRelease(m_pTexture);
      global::dx11::SafeRelease(m_pShaderResourceView);
      global::dx11::SafeRelease(m_pSamplerState);
    }
  }
}

