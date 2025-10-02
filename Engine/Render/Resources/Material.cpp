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
      m_dctTextures.clear();
    }
    // ------------------------------------
    render::texture::CTexture* const CMaterial::GetTexture(texture::ETextureType _eType)
    {
      TMapTextures::iterator it = m_dctTextures.find(_eType);
      return it != m_dctTextures.end() ? it->second : nullptr;
    }
    // ------------------------------------
    render::texture::CTexture* const CMaterial::CreateTexture(texture::ETextureType _eType, std::string _sTextureID)
    {
      // Create new texture or get it
      render::texture::CTexture*& pTargetTexture = m_dctTextures[_eType];
      if (!pTargetTexture)
      {
        pTargetTexture = new render::texture::CTexture(_sTextureID);
      }
      return pTargetTexture;
    }
  }
}
