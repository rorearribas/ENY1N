#pragma once
#include "Libs/Math/Vector3.h"

namespace physics
{
  class CRay
  {
  public:
    CRay() : m_v3Origin(), m_v3Dir() {}
    CRay(const math::CVector3& _v3Origin, const math::CVector3& _v3Dir) :
    m_v3Origin(_v3Origin), m_v3Dir(math::CVector3::Normalize(_v3Dir)) {}

    inline void SetOrigin(const math::CVector3& _v3Origin) { m_v3Origin = _v3Origin; }
    inline const math::CVector3& GetOrigin() const { return m_v3Origin; }

    inline void SetDir(const math::CVector3& _v3Dir) { m_v3Dir = math::CVector3::Normalize(_v3Dir); }
    inline const math::CVector3& GetDir() const { return m_v3Dir; }

    math::CVector3 CalculatePoint(float _fDist) const;
    static math::CVector3 CalculatePoint(const CRay& _oRay, float _fDist);

  private:
    math::CVector3 m_v3Origin;
    math::CVector3 m_v3Dir;
  };
}


