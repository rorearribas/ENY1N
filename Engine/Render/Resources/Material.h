#pragma once
#include "Libs/Math/Vector3.h"
#include "Texture.h"
#include <string>
#include <map>

namespace render
{
  namespace mat
  {
    class CMaterial
    {
    public:
      typedef std::map<texture::ETextureType, texture::CTexture*> TMapTextures;

    public:
      CMaterial(std::string _sMaterialId) : m_sMaterialID(_sMaterialId) {}
      ~CMaterial();

      render::texture::CTexture* const CreateTexture(texture::ETextureType _eType, std::string _sTextureID);
      render::texture::CTexture* const GetTexture(texture::ETextureType _eType);
      const std::string& GetMaterialId() const { return m_sMaterialID; }

      void SetDiffuseColor(math::CVector3 _vDiffuseColor) { m_v3DiffuseColor = _vDiffuseColor; }
      const math::CVector3& GetDiffuseColor() const { return m_v3DiffuseColor; }
      void SetSpecularColor(math::CVector3 _vSpecularColor) { m_v3SpecularColor = _vSpecularColor; }
      const math::CVector3& GetSpecularColor() const { return m_v3SpecularColor; }
      void SetAmbientColor(math::CVector3 _v3AmbientColor) { m_v3AmbientColor = _v3AmbientColor; }
      const math::CVector3& GetAmbientColor() const { return m_v3AmbientColor; }

      void SetEmissiveColor(math::CVector3 _v3Emissive) { m_v3Emissive = _v3Emissive; }
      const math::CVector3& GetEmissiveColor() const { return m_v3Emissive; }
      void SetTransparentColor(math::CVector3 _v3Transparent) { m_v3Transparent = _v3Transparent; }
      const math::CVector3& GetTransparentColor() const { return m_v3Transparent; }
      void SetReflectiveColor(math::CVector3 _v3Reflective) { m_v3Reflective = _v3Reflective; }
      const math::CVector3& GetReflectiveColor() const { return m_v3Reflective; }

      void SetOpacity(float _fTransparent) { m_fOpacity = _fTransparent; }
      const float& GetOpacity() const { return m_fOpacity; }
      void SetShininess(float _fShininess) { m_fShininess = _fShininess; }
      const float& GetShininess() const { return m_fShininess; }

    private:
      // ID
      std::string m_sMaterialID = std::string();

      // Properties
      math::CVector3 m_v3DiffuseColor = math::CVector3::Zero;
      math::CVector3 m_v3SpecularColor = math::CVector3::Zero;
      math::CVector3 m_v3AmbientColor = math::CVector3::Zero;

      math::CVector3 m_v3Emissive = math::CVector3::Zero;
      math::CVector3 m_v3Transparent = math::CVector3::Zero;
      math::CVector3 m_v3Reflective  = math::CVector3::Zero;

      float m_fOpacity = 1.0f;
      float m_fShininess = 1.0f;

      // Textures
      TMapTextures m_dctTextures = TMapTextures();
    };
  }
}




