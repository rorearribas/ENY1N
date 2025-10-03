#include "Texture.h"
#include <iostream>
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace texture
  {
    namespace internal_texture
    {
      static const DXGI_FORMAT s_eTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
    }

    // ------------------------------------
    CTexture::~CTexture()
    {
      Clear();
    }
    // ------------------------------------
    void CTexture::BindTexture()
    {
      // Bind texture to current pipeline
      if (m_pShaderResourceView && m_pSamplerState)
      {
        global::dx11::s_pDeviceContext->PSSetShaderResources(0, 1, &m_pShaderResourceView);
        global::dx11::s_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
      }
    }
    // ------------------------------------
    HRESULT CTexture::SetTexture(void* _pData, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uChannels)
    {
      // Clear
      Clear();

      // Set values
      m_uTextureWidth = _uWidth;
      m_uTextureHeight = _uHeight;
      m_uChannels = _uChannels;

      // Create texture
      D3D11_TEXTURE2D_DESC oTextureDesc = D3D11_TEXTURE2D_DESC();
      oTextureDesc.Width = m_uTextureWidth;
      oTextureDesc.Height = m_uTextureHeight;
      oTextureDesc.MipLevels = 1;
      oTextureDesc.ArraySize = 1;
      oTextureDesc.SampleDesc.Count = 1;
      oTextureDesc.Format = internal_texture::s_eTargetFormat;
      oTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

      D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
      oSubresourceData.pSysMem = _pData;
      oSubresourceData.SysMemPitch = m_uTextureWidth * m_uChannels;

      HRESULT hResult = global::dx11::s_pDevice->CreateTexture2D(&oTextureDesc, &oSubresourceData, &m_pTexture);
      if (FAILED(hResult))
      {
        std::cout << "Error creating texture 2D!" << std::endl;
        return hResult;
      }

      // Create texture view
      D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
      oShaderResourceViewDesc.Format = internal_texture::s_eTargetFormat;
      oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      oShaderResourceViewDesc.Texture2D.MipLevels = 1;
      hResult = global::dx11::s_pDevice->CreateShaderResourceView(m_pTexture, &oShaderResourceViewDesc, &m_pShaderResourceView);
      if (FAILED(hResult))
      {
        std::cout << "Error creating render target view!" << std::endl;
        return hResult;
      }

      // Create sampler state
      D3D11_SAMPLER_DESC oSamplerDesc = D3D11_SAMPLER_DESC();

      // Set config
      oSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
      oSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
      oSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
      oSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
      oSamplerDesc.MipLODBias = 0.0f;
      oSamplerDesc.MaxAnisotropy = 16u;
      oSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
      oSamplerDesc.BorderColor[0] = 0.0f;
      oSamplerDesc.BorderColor[1] = 0.0f;
      oSamplerDesc.BorderColor[2] = 0.0f;
      oSamplerDesc.BorderColor[3] = 0.0f;
      oSamplerDesc.MinLOD = 0.0f;
      oSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

      hResult = global::dx11::s_pDevice->CreateSamplerState(&oSamplerDesc, &m_pSamplerState);
      if (FAILED(hResult))
      {
        std::cout << "Failed to create sampler state!" << std::endl;
        return hResult;
      }

      return hResult;
    }
    // ------------------------------------
    void CTexture::Clear()
    {
      global::dx11::SafeRelease(m_pTexture);
      global::dx11::SafeRelease(m_pShaderResourceView);
      global::dx11::SafeRelease(m_pSamplerState);
    }
  }
}

