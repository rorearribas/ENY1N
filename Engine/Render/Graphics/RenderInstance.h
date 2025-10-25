#pragma once
#include "Libs/Math/Transform.h"
#include "Libs/Math/Vector3.h"

namespace render
{
  namespace gfx
  {
    class CRenderInstance
    {
    public:
      CRenderInstance() {}
      ~CRenderInstance() {}

      inline void SetPosition(const math::CVector3& _v3Position) { m_oTransform.SetPosition(_v3Position); }
      inline const math::CVector3& GetPosition() const { return m_oTransform.GetPosition(); }
      inline void SetRotation(const math::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      inline const math::CVector3& GetRotation() const { return m_oTransform.GetRotation(); }
      inline void SetScale(const math::CVector3& _v3Scale) { m_oTransform.SetScale(_v3Scale); }
      inline const math::CVector3& GetScale() const { return m_oTransform.GetScale(); }

    private:
      math::CTransform m_oTransform;
    };
  }
}


