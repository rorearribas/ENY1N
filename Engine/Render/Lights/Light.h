#pragma once
#include <string>
#include "Libs/Maths/Transform.h"

namespace render
{
  namespace lights
  {
    // Base class
    class CLight
    {
    public:
      CLight() {}
      virtual ~CLight() {}

      void SetPostion(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
      const maths::CVector3& GetPosition() { return m_oTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() { return m_oTransform.GetRotation(); }
      void SetColor(const maths::CVector3& _v3Color) { m_v3LightColor = _v3Color;}
      const maths::CVector3& GetColor() { return m_v3LightColor; }

    private:
      maths::CVector3 m_v3LightColor = maths::CVector3::One;
      maths::CTransform m_oTransform;
    };
  }
}


