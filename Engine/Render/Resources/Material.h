#pragma once
#include "Libs/Maths/Vector3.h"
#include "Texture.h"
#include <string>
#include <map>

namespace render
{
  namespace material
  {
    enum EModifierType : int { AMBIENT, DIFFUSE, SPECULAR, SPECULAR_HIGHLIGHT, BUMP, DISPLACEMENT, ALPHA, REFLECTION, COUNT};

    class CMaterial
    {
    public:
      typedef std::map<EModifierType, std::string> TMapTexturesPath;
      typedef std::map<EModifierType, render::texture::CTexture*> TMapTextures;

    public:
      CMaterial(std::string _sMaterialName) : m_sMaterialName(_sMaterialName) {}
      ~CMaterial();

      const std::string& GetMaterialName() const{ return m_sMaterialName; }
      render::texture::CTexture* const GetTexture(EModifierType _eMapType) { return m_dctTextures[_eMapType]; }
      render::texture::CTexture* const RegisterTexture(EModifierType _eModifierType);

      void SetAmbientColor(maths::CVector3 _vAmbientColor) { m_vAmbientColor = _vAmbientColor; }
      const maths::CVector3& GetAmbientColor() { return m_vAmbientColor; }
      void SetDiffuseColor(maths::CVector3 _vDiffuseColor) { m_vDiffuseColor = _vDiffuseColor; }
      const maths::CVector3& GetDiffuseColor() { return m_vDiffuseColor; }
      void SetSpecularColor(maths::CVector3 _vSpecularColor) { m_vSpecularColor = _vSpecularColor; }
      const maths::CVector3& GetSpecularColor() { return m_vSpecularColor; }

      void SetOpacity(float _fTransparent) { m_fOpacity = _fTransparent; }
      const float& GetOpacity() { return m_fOpacity; }
      void SetOpticalDensity(float _fOpticalDensity) { m_fOpticalDensity = _fOpticalDensity; }
      const float& GetOpticalDensity() { return m_fOpticalDensity; }
      void SetSpecularExponent(float _fSpecularExponent) { m_fSpecularExponent = _fSpecularExponent; }
      const float& GetSpecularExponent() { return m_fSpecularExponent; }

    private:
      std::string m_sMaterialName = {};

      maths::CVector3 m_vAmbientColor = maths::CVector3::Zero; // Ka
      maths::CVector3 m_vDiffuseColor = maths::CVector3::Zero; // Kd
      maths::CVector3 m_vSpecularColor = maths::CVector3::Zero; // Ks

      float m_fOpticalDensity = 1.0f; // Ni
      float m_fSpecularExponent = 1.0f; // Ns
      float m_fOpacity = 1.0f; // d

      int m_iIlluminationVariable = 1; // illum

      TMapTextures m_dctTextures;
    };
  }
}




