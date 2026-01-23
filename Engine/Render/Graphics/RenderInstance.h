#pragma once

#include "Engine/Collisions/AABB.h"
#include "Libs/Math/Transform.h"
#include "Libs/Math/Vector3.h"

namespace render { namespace gfx { class CModel; } }
namespace render
{
  namespace instance
  {
    static constexpr uint16_t s_uInvalidID = 0xFFFFu;
  }

  namespace gfx
  {
    class CRenderInstance
    {
    public:
      CRenderInstance(const render::gfx::CModel* _pParent, uint16_t _uId);
      ~CRenderInstance() {}

      void SetPos(const math::CVector3& _v3Pos);
      inline const math::CVector3& GetPos() const { return m_oTransform.GetPos(); }
      void SetRot(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRot() const { return m_oTransform.GetRot(); }
      void SetScl(const math::CVector3& _v3Scl);
      inline const math::CVector3& GetScl() const { return m_oTransform.GetScl(); }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      inline const math::CTransform& GetTransform() const { return m_oTransform; }
      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }
      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }

      inline const uint16_t GetInstanceID() const { return m_uInstanceID; }
      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

    private:
      const render::gfx::CModel* m_pParent = nullptr;
      uint16_t m_uInstanceID = render::instance::s_uInvalidID;

    private:
      collision::CAABB m_oWorldAABB = collision::CAABB();
      math::CTransform m_oTransform = math::CTransform();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}


