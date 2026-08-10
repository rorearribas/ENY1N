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

    render::texture::TRenderTarget& GetTexture() { return m_oRTTexture; }
    ID3D11ShaderResourceView* GetShaderView() { return m_oBindedTexture.GetView(); }

  private:
    render::texture::TRenderTarget m_oRTTexture;
    render::texture::TShaderResource m_oBindedTexture;
  };
}