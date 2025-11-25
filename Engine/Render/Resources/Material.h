#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Resources/Texture2D.h"

#include "Libs/Math/Vector3.h"
#include <string>
#include <array>

namespace render
{
  namespace mat
  {
    class CMaterial
    {
    private:
      typedef std::array<texture::TSharedTexture, s_uTextureCount> TTextures;

    public:
      CMaterial() = default;
      CMaterial(const std::string& _sMaterialId) : m_sID(_sMaterialId) {}
      ~CMaterial();

      void SetTexture(texture::TSharedTexture _pTexture, ETexture _eType);
      texture::TSharedTexture GetTexture(ETexture _eType) const;
      inline const std::string& GetID() const { return m_sID; }

      inline void SetDiffuseColor(math::CVector3 _vDiffuseColor) { m_v3DiffuseColor = _vDiffuseColor; }
      inline const math::CVector3& GetDiffuseColor() const { return m_v3DiffuseColor; }
      inline void SetSpecularColor(math::CVector3 _vSpecularColor) { m_v3SpecularColor = _vSpecularColor; }
      inline const math::CVector3& GetSpecularColor() const { return m_v3SpecularColor; }
      inline void SetAmbientColor(math::CVector3 _v3AmbientColor) { m_v3AmbientColor = _v3AmbientColor; }
      inline const math::CVector3& GetAmbientColor() const { return m_v3AmbientColor; }

      inline void SetEmissiveColor(math::CVector3 _v3Emissive) { m_v3Emissive = _v3Emissive; }
      inline const math::CVector3& GetEmissiveColor() const { return m_v3Emissive; }
      inline void SetTransparentColor(math::CVector3 _v3Transparent) { m_v3Transparent = _v3Transparent; }
      inline const math::CVector3& GetTransparentColor() const { return m_v3Transparent; }
      inline void SetReflectiveColor(math::CVector3 _v3Reflective) { m_v3Reflective = _v3Reflective; }
      inline const math::CVector3& GetReflectiveColor() const { return m_v3Reflective; }

      inline void SetOpacity(float _fTransparent) { m_fOpacity = _fTransparent; }
      inline const float& GetOpacity() const { return m_fOpacity; }
      inline void SetShininess(float _fShininess) { m_fShininess = _fShininess; }
      inline const float& GetShininess() const { return m_fShininess; }

    private:
      void ClearTextures();

    private:
      // ID
      std::string m_sID = std::string();

      // Properties
      math::CVector3 m_v3DiffuseColor = math::CVector3::Zero;
      math::CVector3 m_v3SpecularColor = math::CVector3::Zero;
      math::CVector3 m_v3AmbientColor = math::CVector3::Zero;

      math::CVector3 m_v3Emissive = math::CVector3::Zero;
      math::CVector3 m_v3Transparent = math::CVector3::Zero;
      math::CVector3 m_v3Reflective = math::CVector3::Zero;

      float m_fOpacity = 1.0f;
      float m_fShininess = 1.0f;

      // Textures
      TTextures m_lstTextures = TTextures();
    };
  }
}




