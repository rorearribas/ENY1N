#pragma once

#include "Engine/Collisions/AABB.h"
#include "Engine/Render/Resources/Texture2D.h"
#include "Libs/Math/Transform.h"
#include "Libs/Math/Vector3.h"

namespace render
{
  namespace gfx
  {
    class CDecal
    {
    public:
      CDecal() = default;
      ~CDecal() { Release(); }

      CDecal(CDecal&& _rOther) noexcept;
      CDecal& operator=(CDecal&& _rOther) noexcept;
      CDecal(const CDecal& _rOther) = delete;
      CDecal& operator=(const CDecal& _rOther) = delete;

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPos() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRot() const { return m_oTransform.GetRot(); }

      void SetCullEnabled(bool _bCull);
      inline const bool IsCullEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool IsVisible() const { return m_bVisible; }

      void SetBoundingBox(const collision::CAABB& _rBoundingBox);
      inline const collision::CAABB& GetBoundingBox() const { return m_oLocalAABB; }
      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }

      inline const math::CTransform& GetTransform() const { return m_oTransform; }
      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }

    private:
      void Release();

    private:
      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oLocalAABB = collision::CAABB();
      collision::CAABB m_oWorldAABB = collision::CAABB();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;

      // Texture
      render::texture::TShaderResource m_oDecalTexture;
    };
  }
}
