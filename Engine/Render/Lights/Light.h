#pragma once
#include <string>
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace lights
  {
    enum ELightType { DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT };

    // Base class
    class CLight
    {
    public:
      CLight() {}
      virtual ~CLight() {}

      virtual void UpdateLight() {} 

      void SetPostion(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
      const maths::CVector3& GetPosition() { return m_oTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() { return m_oTransform.GetRotation(); }

    private:
      // Light transformation
      maths::CTransform m_oTransform;
    };
  }
}


