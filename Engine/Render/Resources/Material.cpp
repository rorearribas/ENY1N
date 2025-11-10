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
    texture::TSharedTexture const CMaterial::GetTexture(ETextureType _eType)
    {
      TMapTextures::iterator it = m_dctTextures.find(_eType);
      return it != m_dctTextures.end() ? it->second : nullptr;
    }
    // ------------------------------------
    void CMaterial::SetTexture(texture::TSharedTexture _pTexture, ETextureType _eType)
    {
      m_dctTextures[_eType] = _pTexture;
    }
    // ------------------------------------
    void CMaterial::ClearTextures()
    {
      auto it = m_dctTextures.begin();
      for (; it != m_dctTextures.end(); ++it)
      {
        it->second.reset();
      }
      m_dctTextures.clear();
    }
  }
}
