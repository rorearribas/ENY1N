#pragma once
#include "Libs/Math/Vector3.h"
#include "Texture.h"
#include <string>
#include <map>

namespace render
{
  namespace material
  {
    // Type of material
    class CMaterial
    {
    public:
      enum EType : uint32_t
      {
        AMBIENT,
        DIFFUSE,
        SPECULAR,
        SPECULAR_HIGHLIGHT,
        BUMP,
        DISPLACEMENT,
        ALPHA,
        REFLECTION,
        COUNT
      };
      typedef std::map<CMaterial::EType, texture::CTexture*> TMapTextures;

    public:
      CMaterial(std::string _sMaterialId) : m_sMaterialId(_sMaterialId) {}
      ~CMaterial();

      render::texture::CTexture* const RegisterTexture(CMaterial::EType _eType, std::string _sTextureID);
      render::texture::CTexture* const GetTexture(CMaterial::EType _eType);
      const std::string& GetMaterialId() const { return m_sMaterialId; }

      void SetAmbientColor(math::CVector3 _vAmbientColor) { m_vAmbientColor = _vAmbientColor; }
      const math::CVector3& GetAmbientColor() const { return m_vAmbientColor; }
      void SetDiffuseColor(math::CVector3 _vDiffuseColor) { m_vDiffuseColor = _vDiffuseColor; }
      const math::CVector3& GetDiffuseColor() const { return m_vDiffuseColor; }
      void SetSpecularColor(math::CVector3 _vSpecularColor) { m_vSpecularColor = _vSpecularColor; }
      const math::CVector3& GetSpecularColor() const { return m_vSpecularColor; }

      void SetOpacity(float _fTransparent) { m_fOpacity = _fTransparent; }
      const float& GetOpacity() const { return m_fOpacity; }
      void SetOpticalDensity(float _fOpticalDensity) { m_fOpticalDensity = _fOpticalDensity; }
      const float& GetOpticalDensity() const { return m_fOpticalDensity; }
      void SetSpecularExponent(float _fSpecularExponent) { m_fSpecularExponent = _fSpecularExponent; }
      const float& GetSpecularExponent() const { return m_fSpecularExponent; }

    private:
      TMapTextures m_dctTextures = TMapTextures();
      std::string m_sMaterialId = {};

      math::CVector3 m_vAmbientColor = math::CVector3::Zero; // Ka
      math::CVector3 m_vDiffuseColor = math::CVector3::Zero; // Kd
      math::CVector3 m_vSpecularColor = math::CVector3::Zero; // Ks

      float m_fOpticalDensity = 1.0f; // Ni
      float m_fSpecularExponent = 1.0f; // Ns
      float m_fOpacity = 1.0f; // d

      int m_iIlluminationVariable = 1; // illum
    };
  }
}




