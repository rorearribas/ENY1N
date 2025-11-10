#pragma once

#include "Engine/Collisions/BoundingBox.h"
#include "Libs/Math/Transform.h"
#include "Libs/Math/Vector3.h"

namespace render { namespace gfx { class CModel; } }
namespace render
{
  namespace gfx
  {
    class CRenderInstance
    {
    private:
      static constexpr uint32_t s_uInvalidID = 0xFFFFFFFFu;

    public:
      CRenderInstance(CModel* _pModel, uint32_t _uId);
      ~CRenderInstance() {}

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

      inline void SetTransform(const math::CTransform& _oTransform) { m_oTransform = _oTransform; }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }
      inline math::CMatrix4x4 GetMatrix() const { return m_oTransform.GetMatrix(); }

      void SetPosition(const math::CVector3& _v3Position);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }
      inline const uint32_t GetInstanceID() const { return m_uInstanceID; }

    private:
      render::gfx::CModel* m_pModel = nullptr;
      uint32_t m_uInstanceID = s_uInvalidID;

    private:
      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox = collision::CBoundingBox();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}


