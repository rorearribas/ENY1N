#include "Texture2D.h"
#include <iostream>
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace texture
  {
    //// ------------------------------------
    //void CTexture::AttachTexture()
    //{
    //  // Attach texture to current pipeline
    //  if (m_pResourceView && m_pSamplerState)
    //  {
    //    global::dx::s_pDeviceContext->PSSetShaderResources(0, 1, &m_pResourceView);
    //    global::dx::s_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
    //  }
    //}
    //// ------------------------------------
    //template<EViewType T>
    //void CTexture<T>::DetachTexture()
    //{
    //  // Detach texture to current pipeline
    //  if (m_pResourceView && m_pSamplerState)
    //  {
    //    global::dx::s_pDeviceContext->PSSetShaderResources(0, 1, nullptr);
    //    global::dx::s_pDeviceContext->PSSetSamplers(0, 1, nullptr);
    //  }
    //}
    //// ------------------------------------
    //void CTexture::FlushTexture()
    //{
    //  ClearTexture();
    //  ClearSampler();
    //}
    //// ------------------------------------
    //HRESULT CTexture::CreateTexture(void* _pData, const D3D11_TEXTURE2D_DESC& _oTextureCfg, uint32_t _uChannels)
    //{
    //  // Clear
    //  ClearTexture();

    //  // Create texture using raw data!
    //  D3D11_SUBRESOURCE_DATA oSubresourceData = D3D11_SUBRESOURCE_DATA();
    //  oSubresourceData.pSysMem = _pData;
    //  oSubresourceData.SysMemPitch = _oTextureCfg.Width * _uChannels;

    //  return global::dx::s_pDevice->CreateTexture2D(&_oTextureCfg, &oSubresourceData, &m_pTexture);

    //  //// Create texture view
    //  //D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    //  //oShaderResourceViewDesc.Format = _oTextureCfg.Format;
    //  //oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //  //oShaderResourceViewDesc.Texture2D.MipLevels = 1;
    //  //return global::dx::s_pDevice->CreateShaderResourceView(m_pTexture, &oShaderResourceViewDesc, &m_pResourceView);
    //}
    //// ------------------------------------
    //HRESULT CTexture::CreateTexture(const D3D11_TEXTURE2D_DESC& _oTextureCfg)
    //{
    //  // Clear
    //  ClearTexture();

    //  // Clear empty texture!
    //  HRESULT hResult = global::dx::s_pDevice->CreateTexture2D(&_oTextureCfg, nullptr, &m_pTexture);
    //  if (FAILED(hResult))
    //  {
    //    std::cout << "Error creating texture 2D!" << std::endl;
    //    return hResult;
    //  }

    //  // Create texture view
    //  D3D11_SHADER_RESOURCE_VIEW_DESC oShaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    //  oShaderResourceViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //  oShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //  oShaderResourceViewDesc.Texture2D.MipLevels = 1;
    //  return global::dx::s_pDevice->CreateShaderResourceView(m_pTexture, &oShaderResourceViewDesc, &m_pResourceView);
    //}
    //// ------------------------------------
    //HRESULT CTexture::CreateSampler(const D3D11_SAMPLER_DESC& _oSamplerCfg)
    //{
    //  ClearSampler();
    //  return global::dx::s_pDevice->CreateSamplerState(&_oSamplerCfg, &m_pSamplerState);
    //}
    //// ------------------------------------
    //void CTexture::ClearTexture()
    //{
    //  global::dx::SafeRelease(m_pTexture);
    //  global::dx::SafeRelease(m_pResourceView);
    //}
    //// ------------------------------------
    //void CTexture::ClearSampler()
    //{
    //  global::dx::SafeRelease(m_pSamplerState);
    //}
  }
}

