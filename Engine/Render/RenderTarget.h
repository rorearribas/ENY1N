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
    CRenderTarget() = default;
    ~CRenderTarget() { CleanRT(); }

    HRESULT CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eTargetFormat);
    void ClearRT(const float _v4ClearColor[4]);

    ID3D11Texture2D* GetTexture() const { return *m_pTexture; }
    ID3D11RenderTargetView* GetRT() const { return m_pTexture->GetView(); }
    ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }

    // Override operators
    inline operator ID3D11RenderTargetView* () const { return GetRT(); }
    inline operator const ID3D11RenderTargetView* () const { return GetRT(); }

  private:
    void CleanRT();

  private:
    // Data
    texture::CTexture2D<render::EViewType::RENDER_TARGET>* m_pTexture = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;
  };
}