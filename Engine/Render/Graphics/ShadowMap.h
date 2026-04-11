#pragma once
#include "Engine/Render/Resources/Texture2D.h"

namespace render
{
  namespace gfx
  {
    class CShadowMap
    {
    public:
      CShadowMap() {}
      ~CShadowMap();

      HRESULT Setup(uint32_t _uWidth, uint32_t _uHeight);
      const texture::TDepthStencil& GetShadowDepth() const { return m_oShadowDepth; }
      const texture::TShaderResource& GetShadowTexture() const { return m_oShadowTexture; }

    private:
      texture::TDepthStencil m_oShadowDepth;
      texture::TShaderResource m_oShadowTexture;
    };
  }
}

