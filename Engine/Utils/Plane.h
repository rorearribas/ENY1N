#pragma once
#include "Libs/Math/Vector3.h"

namespace math
{
  class CPlane
  {
  public:
    CPlane() : m_v3Point(math::CVector3::Zero), m_v3Normal(math::CVector3::Zero) {}
    CPlane(const math::CVector3& _v3Pos, const math::CVector3& _v3Normal) : m_v3Point(_v3Pos), m_v3Normal(_v3Normal) {}
    ~CPlane() {}

    inline void SetPos(const math::CVector3& _v3Pos) { m_v3Point = _v3Pos; }
    inline const math::CVector3& GetPos() const { return m_v3Point; }
    inline void SetNormal(const math::CVector3& _v3Normal) { m_v3Normal = math::CVector3::Normalize(_v3Normal); }
    inline const math::CVector3& GetNormal() const { return m_v3Normal; }

    inline void Set(const math::CVector3& _v3Pos, const math::CVector3& _v3Normal) { SetPos(_v3Pos); SetNormal(_v3Normal); }
    float DistanceToPoint(const math::CVector3& _v3Point) const;
    static float DistanceToPoint(const math::CPlane& _oPlane, const math::CVector3& _v3Point);

  private:
    math::CVector3 m_v3Point;
    math::CVector3 m_v3Normal;
  };
}


