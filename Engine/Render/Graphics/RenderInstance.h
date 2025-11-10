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
    public:
      CRenderInstance(render::gfx::CModel* _pModel) : m_pModel(_pModel) {}
      ~CRenderInstance() {}

      void Draw();

      inline void SetBoundingBox(const collision::CBoundingBox& _oBoundingBox) { m_oBoundingBox = _oBoundingBox; }
      inline const collision::CBoundingBox& GetBoundingBox() const { return m_oBoundingBox; }

      void SetCullingEnabled(bool _bCull);
      inline const bool& IsCullingEnabled() const { return m_bCullEnabled; }

      inline void SetVisible(bool _bVisible) { m_bVisible = _bVisible; }
      inline const bool& IsVisible() const { return m_bVisible; }

      inline void SetTransform(const math::CTransform& _oTransform) { m_oTransform = _oTransform; }
      inline const math::CTransform& GetTransform() const { return m_oTransform; }

      void SetPosition(const math::CVector3& _v3Position);
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      void SetRotation(const math::CVector3& _v3Rot);
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      void SetScale(const math::CVector3& _v3Scale);
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    private:
      render::gfx::CModel* m_pModel = nullptr;

    private:
      math::CTransform m_oTransform = math::CTransform();
      collision::CBoundingBox m_oBoundingBox = collision::CBoundingBox();

      bool m_bCullEnabled = true;
      bool m_bVisible = true;
    };
  }
}


