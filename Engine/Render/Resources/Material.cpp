#include "Material.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/Macros/GlobalMacros.h"
#include <iostream>

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
      m_dctTexturesPath.clear();
      m_dctTextures.clear();
    }
    // ------------------------------------
    void CMaterial::LoadTexture(EModifierType _eModifierType, const char* _sPath)
    {
      render::texture::CTexture*& pTargetTexture = m_dctTextures[_eModifierType];
      SafeRelease(pTargetTexture);
      pTargetTexture = new render::texture::CTexture(_sPath); // create texture
    }
    // ------------------------------------
    void CMaterial::RegisterPath(EModifierType _eModifierType, std::string _sPath)
    {
      // Set texture path
      m_dctTexturesPath[_eModifierType] = _sPath;
      // Try to load the texture if the path is not empty
      if (!_sPath.empty())
      {
        LoadTexture(_eModifierType, _sPath.c_str());
      }
    }
  }
}
