#pragma once
#include "Libs/Maths/Vector3.h"
#include "Texture.h"
#include <string>
#include <map>

namespace render
{
  namespace material
  {
    enum EModifierType { AMBIENT, DIFFUSE, SPECULAR, ALPHA, BUMP };

    class CMaterial
    {
    public:
      typedef std::map<EModifierType, std::string> TMapTexturesPath;
      typedef std::map<EModifierType, render::texture::CTexture*> TMapTextures;

    public:
      CMaterial(const char* _sMaterialName) : m_sMaterialName(_sMaterialName) {}

      void LoadTexture(EModifierType /*_eMapType*/, const std::string& /*_sPath*/) {}
      render::texture::CTexture* const GetTexture(EModifierType _eMapType) { return m_dctTextures[_eMapType]; }
      const std::string& GetPath(EModifierType _eMapType) { return m_dctTexturesPath[_eMapType]; }

      void SetAmbientColor(maths::CVector3 _vAmbientColor) { m_vAmbientColor = _vAmbientColor; }
      void SetDiffuseColor(maths::CVector3 _vDiffuseColor) { m_vDiffuseColor = _vDiffuseColor; }
      void SetSpecularColor(maths::CVector3 _vSpecularColor) { m_vSpecularColor = _vSpecularColor; }

      void SetTransparent(float _fTransparent) { m_fOpacity = _fTransparent; }
      void SetOpticalDensity(float _fOpticalDensity) { m_fOpticalDensity = _fOpticalDensity; }
      void SetSpecularExponent(float _fSpecularExponent) { m_fSpecularExponent = _fSpecularExponent; }

    private:
      std::string m_sMaterialName = {};

      maths::CVector3 m_vAmbientColor = maths::CVector3::Zero; // Ka
      maths::CVector3 m_vDiffuseColor = maths::CVector3::Zero; // Kd
      maths::CVector3 m_vSpecularColor = maths::CVector3::Zero; // Ks

      float m_fOpticalDensity = 1.0f; // Ni
      float m_fSpecularExponent = 1.0f; // Ns
      float m_fOpacity = 1.0f; // d

      int m_iIlluminationVariable = 1; // illum

      TMapTexturesPath m_dctTexturesPath;
      TMapTextures m_dctTextures;
    };
  }
}



