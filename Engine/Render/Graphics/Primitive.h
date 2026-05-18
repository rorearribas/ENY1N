#pragma once
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Collisions/AABB.h"

#include "Libs/Math/Vector3.h"
#include "Libs/Math/Matrix4x4.h"
#include "Libs/Math/Transform.h"

#include <vector>

namespace render
{
  namespace gfx
  {
    class CPrimitive
    {
    public:
      CPrimitive() = default;
      ~CPrimitive() = default;

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPos() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRot() const { return m_oTransform.GetRot(); }
      void SetScl(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScl() const { return m_oTransform.GetScl(); }
      inline void SetColor(const math::CVector3& _v3Color) { m_v3Color = _v3Color; }
      inline const math::CVector3& GetColor() const { return m_v3Color; }

      void SetCullEnabled(bool _bCull);
      inline const bool IsCullEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool IsVisible() const { return m_bVisible; }

      inline void SetRenderMode(render::ERenderMode _eRenderMode) { m_eRenderMode = _eRenderMode; }
      inline const ERenderMode& GetRenderMode() const { return m_eRenderMode; }

      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }

      inline void SetLocalAABB(const collision::CAABB& _rLocalAABB) { m_oLocalAABB = _rLocalAABB; }
      inline const collision::CAABB& GetLocalAABB() const { return m_oLocalAABB; }
      inline void SetWorldAABB(const collision::CAABB& _rWorldAABB) { m_oWorldAABB = _rWorldAABB; }
      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }

      inline const uint32_t& GetIdxCount() const { return m_uIndexCount; }
      inline void SetIdxCount(uint32_t _uIndexCount) { m_uIndexCount = _uIndexCount; }
      inline const uint32_t& GetVtxOffset() const { return m_uVertexOffset; }
      inline void SetVtxOffset(const uint32_t& _uVertexOffset) { m_uVertexOffset = _uVertexOffset; }
      inline const uint32_t& GetIdxOffset() const { return m_uIndexOffset; }
      inline void SetIdxOffset(const uint32_t& _uIndexOffset) { m_uIndexOffset = _uIndexOffset; }

    private:
      // Data
      ERenderMode m_eRenderMode = ERenderMode::SOLID;
      math::CVector3 m_v3Color = math::CVector3::One;

      math::CTransform m_oTransform = math::CTransform();
      collision::CAABB m_oWorldAABB = collision::CAABB();
      collision::CAABB m_oLocalAABB = collision::CAABB();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;

      uint32_t m_uIndexCount = 0;
      uint32_t m_uVertexOffset = 0;
      uint32_t m_uIndexOffset = 0;
    };
  }
}
