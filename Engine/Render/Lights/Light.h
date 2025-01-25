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
      CLight(const std::string& _sName) : m_sLightName(_sName) {}
      virtual ~CLight() {}

      virtual void Update() {};

      void SetPostion(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
      const maths::CVector3& GetPosition() { return m_oTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() { return m_oTransform.GetRotation(); }
      void SetColor(const maths::CVector3& _v3Color) { m_vLightColor = _v3Color;}
      const maths::CVector3& GetColor() { return m_vLightColor; }

    private:
      std::string m_sLightName;
      maths::CVector3 m_vLightColor = maths::CVector3::One;
      maths::CTransform m_oTransform;
    };
  }
}


