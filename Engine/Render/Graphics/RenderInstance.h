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

      inline const uint16_t GetInstanceID() const { return m_uInstanceID; }
      inline const collision::CAABB& GetWorldAABB() const { return m_oWorldAABB; }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

      inline void SetTransform(const math::CTransform& _oTransform) { m_oTransform = _oTransform; }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }
      inline const math::CMatrix4x4& GetMatrix() const { return m_oTransform.GetMatrix(); }

      void SetPosition(const math::CVector3& _v3Position);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetTranslation(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

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


