#pragma once
#include "Libs/Maths/Vector3.h"
#include "Texture.h"
#include <string>
#include <map>

namespace render
{
  namespace material
  {
    // Type of material
    enum EModifierType : uint32_t { AMBIENT, DIFFUSE, SPECULAR, SPECULAR_HIGHLIGHT, BUMP, DISPLACEMENT, ALPHA, REFLECTION, COUNT };

    class CMaterial
    {
    public:
      typedef std::map<EModifierType, render::texture::CTexture*> TMapTextures;

    public:
      CMaterial(std::string _sMaterialId) : m_sMaterialId(_sMaterialId) {}
      ~CMaterial();

      render::texture::CTexture* const RegisterTexture(EModifierType _eModifierType, std::string _sTextureID);
      render::texture::CTexture* const GetTexture(EModifierType _eModifierType);
      const std::string& GetMaterialId() const { return m_sMaterialId; }

      void SetAmbientColor(maths::CVector3 _vAmbientColor) { m_vAmbientColor = _vAmbientColor; }
      const maths::CVector3& GetAmbientColor() const { return m_vAmbientColor; }
      void SetDiffuseColor(maths::CVector3 _vDiffuseColor) { m_vDiffuseColor = _vDiffuseColor; }
      const maths::CVector3& GetDiffuseColor() const { return m_vDiffuseColor; }
      void SetSpecularColor(maths::CVector3 _vSpecularColor) { m_vSpecularColor = _vSpecularColor; }
      const maths::CVector3& GetSpecularColor() const { return m_vSpecularColor; }

      void SetOpacity(float _fTransparent) { m_fOpacity = _fTransparent; }
      const float& GetOpacity() const { return m_fOpacity; }
      void SetOpticalDensity(float _fOpticalDensity) { m_fOpticalDensity = _fOpticalDensity; }
      const float& GetOpticalDensity() const { return m_fOpticalDensity; }
      void SetSpecularExponent(float _fSpecularExponent) { m_fSpecularExponent = _fSpecularExponent; }
      const float& GetSpecularExponent() const { return m_fSpecularExponent; }

    private:
      TMapTextures m_dctTextures = TMapTextures();
      std::string m_sMaterialId = {};

      maths::CVector3 m_vAmbientColor = maths::CVector3::Zero; // Ka
      maths::CVector3 m_vDiffuseColor = maths::CVector3::Zero; // Kd
      maths::CVector3 m_vSpecularColor = maths::CVector3::Zero; // Ks

      float m_fOpticalDensity = 1.0f; // Ni
      float m_fSpecularExponent = 1.0f; // Ns
      float m_fOpacity = 1.0f; // d

      int m_iIlluminationVariable = 1; // illum
    };
  }
}




