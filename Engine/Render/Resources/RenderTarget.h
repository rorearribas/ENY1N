#pragma once
#include <d3d11.h>
#include "Engine/Render/Resources/Texture2D.h"
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

    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight, DXGI_FORMAT _eFormat);
    void SetClearColor(const float _v4ClearColor[4]);
    void Release();

    ID3D11Texture2D* GetTexture() const { return m_oRTTexture; }
    ID3D11RenderTargetView* GetRenderTargetView() const { return m_oRTTexture.GetView(); }
    ID3D11ShaderResourceView* GetRTView() const { return m_pShaderView; }

  private:
    texture::CTexture2D<render::EView::RENDER_TARGET> m_oRTTexture;
    ID3D11ShaderResourceView* m_pShaderView = nullptr;
  };
}