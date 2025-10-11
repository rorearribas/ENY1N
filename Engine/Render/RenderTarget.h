#pragma once
#include <d3d11.h>
#include <string>
#include "Resources/Texture.h"

namespace render
{
  class CRenderTarget
  {
  public:
    CRenderTarget(const std::string& _sRenderTargetID) : 
    m_sRenderTargetID(_sRenderTargetID) {}
    ~CRenderTarget() { CleanRT(); }

    HRESULT CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eTargetFormat);
    void AttachRT();
    void DetachRT();

    texture::CTexture<texture::RENDER_TARGET>* GetTexture() const { return m_pTexture; }
    ID3D11RenderTargetView* GetRT() const { return m_pTexture->GetResourceView(); }

  private:
    void CleanRT();

  private:
    // Info
    std::string m_sRenderTargetID = std::string();
    texture::CTexture<texture::RENDER_TARGET>* m_pTexture = nullptr;
  };
}

