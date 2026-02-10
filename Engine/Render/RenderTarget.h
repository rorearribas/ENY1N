#pragma once
#include <d3d11.h>
#include "Resources/Texture2D.h"
#include "Engine/Render/RenderTypes.h"

namespace render
{
  class CRenderTarget
  {
  public:
    CRenderTarget() = default;
    ~CRenderTarget() { Release(); }

    CRenderTarget(const CRenderTarget&) = delete;
    CRenderTarget& operator=(const CRenderTarget&) = delete;

    HRESULT CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eFormat);
    void ClearRT(const float _v4ClearColor[4]);
    void Release();

    ID3D11Texture2D* GetTexture() const { return m_oTexture; }
    ID3D11ShaderResourceView* GetView() const { return m_pSRV; }

    inline operator ID3D11RenderTargetView* () const { return m_oTexture.GetView(); }
    inline operator const ID3D11RenderTargetView* () const { return m_oTexture.GetView(); }

  private:
    texture::CTexture2D<render::EView::RENDER_TARGET> m_oTexture;
    ID3D11ShaderResourceView* m_pSRV = nullptr;
  };
}