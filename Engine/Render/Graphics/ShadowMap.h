#pragma once
#include "Engine/Render/Resources/Texture2D.h"

namespace render
{
  namespace gfx
  {
    class CShadowMap
    {
    public:
      CShadowMap() = default;
      ~CShadowMap() { Release(); }

      CShadowMap(CShadowMap&& _rOther) noexcept;
      CShadowMap& operator=(CShadowMap&& _rOther) noexcept;
      CShadowMap(const CShadowMap& _rOther) = delete;
      CShadowMap& operator=(const CShadowMap& _rOther) = delete;

      HRESULT Setup(uint32_t _uWidth, uint32_t _uHeight);
      const texture::TDepthStencil& GetStencil() const { return m_oShadowDepth; }
      const texture::TShaderResource& GetTexture() const { return m_oShadowTexture; }

    private:
      void Release();
      render::texture::TDepthStencil m_oShadowDepth;
      render::texture::TShaderResource m_oShadowTexture;
    };
  }
}

