#pragma once
#include <d3d11.h>
#include <string>
#include "Resources/Texture2D.h"
#include "Engine/Render/RenderTypes.h"

namespace render
{
  class CRenderTarget
  {
  public:
    CRenderTarget(const std::string& _sRenderTargetID) : 
    m_sRenderTargetID(_sRenderTargetID) {}
    ~CRenderTarget() { CleanRT(); }

    HRESULT CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eTargetFormat);
    ID3D11Texture2D* GetTexture() const { return *m_pTexture; }
    ID3D11RenderTargetView* GetRT() const { return m_pTexture->GetView(); }

    // Override operators
    inline operator ID3D11RenderTargetView* () const { return GetRT(); }
    inline operator const ID3D11RenderTargetView* () const { return GetRT(); }

  private:
    void CleanRT();

  private:
    // Info
    std::string m_sRenderTargetID = std::string();
    texture::CTexture2D<RENDER_TARGET>* m_pTexture = nullptr;
  };
}

