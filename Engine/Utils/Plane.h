#pragma once
#include "Libs/Math/Vector3.h"

namespace math
{
  class CPlane
  {
  public:
    CPlane(const math::CVector3& _v3Pos, const math::CVector3& _v3Normal) :
      m_v3Pos(_v3Pos), m_v3Normal(_v3Normal) {
    }
    ~CPlane() {}

    void SetPos(const math::CVector3& _v3Pos) { m_v3Pos = _v3Pos; }
    const math::CVector3& GetPos() const { return m_v3Pos; }
    void SetNormal(const math::CVector3& _v3Normal) { m_v3Normal = _v3Normal; }
    const math::CVector3& GetNormal() const { return m_v3Normal; }

    float DistanceToPoint(const math::CVector3& _v3Point) const;
    static float DistanceToPoint(const math::CPlane& _oPlane, const math::CVector3& _v3Point);

  private:
    math::CVector3 m_v3Pos;
    math::CVector3 m_v3Normal;
  };
}


