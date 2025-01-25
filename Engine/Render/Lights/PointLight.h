#pragma once
#include "Light.h"

namespace render
{
  namespace lights
  {
    class CPointLight : public CLight
    { 
    public:
      CPointLight() : CLight(std::string()) {}
      CPointLight(const std::string& _sLightName) : CLight(_sLightName) {}
      virtual ~CPointLight() {}

      void SetPostion(const maths::CVector3& _v3Pos) { m_oTransform.SetPosition(_v3Pos); }
      const maths::CVector3& GetPosition() { return m_oTransform.GetPosition(); }
      void SetRotation(const maths::CVector3& _v3Rot) { m_oTransform.SetRotation(_v3Rot); }
      const maths::CVector3& GetRotation() { return m_oTransform.GetRotation(); }

    private:
      maths::CTransform m_oTransform;
    };
  }

}

