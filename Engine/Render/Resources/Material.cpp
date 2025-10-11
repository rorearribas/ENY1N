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
    render::texture::CTexture<texture::SHADER_RESOURCE>* const CMaterial::GetTexture(texture::ETextureType _eType)
    {
      TMapTextures::iterator it = m_dctTextures.find(_eType);
      return it != m_dctTextures.end() ? it->second : nullptr;
    }
    // ------------------------------------
    render::texture::CTexture<texture::SHADER_RESOURCE>* const CMaterial::RegisterTexture(texture::ETextureType _eType, std::string _sTextureID)
    {
      // Create new texture or get it
      render::texture::CTexture<texture::SHADER_RESOURCE>*& pTargetTexture = m_dctTextures[_eType];
      if (!pTargetTexture)
      {
        pTargetTexture = new render::texture::CTexture<texture::SHADER_RESOURCE>(_sTextureID);
      }
      return pTargetTexture;
    }
    // ------------------------------------
    void CMaterial::ClearTextures()
    {
      auto it = m_dctTextures.begin();
      for (; it != m_dctTextures.end(); ++it)
      {
        global::ReleaseObject(it->second);
      }
      m_dctTextures.clear();
    }
  }
}
