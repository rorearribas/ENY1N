#include "Material.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>
#include <filesystem>

namespace render
{
	namespace material
	{
    template<typename T>
    inline void SafeRelease(T*& pPtr_)
    {
      if (pPtr_)
      {
        delete pPtr_;
        pPtr_ = nullptr;
      }
    }
    // ------------------------------------
    CMaterial::~CMaterial()
    {
      m_dctTextures.clear();
    }

    render::texture::CTexture* const CMaterial::GetTexture(EModifierType _eMapType)
    {
      TMapTextures::iterator it = m_dctTextures.find(_eMapType);
      return it != m_dctTextures.end() ? it->second : nullptr;
    }

    // ------------------------------------
    render::texture::CTexture* const CMaterial::RegisterTexture(EModifierType _eModifierType, std::string _sTextureID)
    {
      // Create texture or get it
      render::texture::CTexture*& pTargetTexture = m_dctTextures[_eModifierType];
      if (!pTargetTexture) { pTargetTexture = new render::texture::CTexture(_sTextureID); }
      return pTargetTexture;
    }
  }
}
