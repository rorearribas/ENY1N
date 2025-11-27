#include "Material.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>
#include <filesystem>

namespace render
{
  namespace mat
  {
    CMaterial::~CMaterial()
    {
      ClearTextures();
    }
    // ------------------------------------
    render::texture::TSharedTexture CMaterial::GetTexture(ETexture _eType) const
    {
      return m_lstTextures[static_cast<uint32_t>(_eType)];
    }
    // ------------------------------------
    void CMaterial::SetTexture(render::texture::TSharedTexture _pTexture, ETexture _eType)
    {
      m_lstTextures[static_cast<uint32_t>(_eType)] = _pTexture;
    }
    // ------------------------------------
    void CMaterial::ClearTextures()
    {
      uint32_t uIndex = 0;
      while (uIndex != s_uTextureCount)
      {
        m_lstTextures[uIndex].reset();
        uIndex++;
      }
    }
  }
}
