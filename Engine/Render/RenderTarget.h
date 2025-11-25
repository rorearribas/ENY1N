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

    HRESULT CreateRT(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eFormat);
    void ClearRT(const float _v4ClearColor[4]);

    ID3D11Texture2D* GetTexture() const { return m_pRTTexture->GetData(); }
    ID3D11RenderTargetView* GetRT() const { return m_pRTTexture->GetView(); }
    ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }

    // Override operators
    inline operator ID3D11RenderTargetView* () const { return GetRT(); }
    inline operator const ID3D11RenderTargetView* () const { return GetRT(); }

  private:
    void CleanRT();

  private:
    texture::CTexture2D<render::EView::RENDER_TARGET>* m_pRTTexture = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;
  };
}