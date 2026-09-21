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
      const texture::TDepthStencil& GetDepthStencil() const { return m_oDepthStencil; }
      const texture::TShaderResource& GetShaderResource() const { return m_oShaderResource; }

    private:
      void Release();

    private:
      texture::TDepthStencil m_oDepthStencil;
      texture::TShaderResource m_oShaderResource;
    };
  }
}

