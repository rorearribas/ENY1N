#pragma once
#include "Collider.h"
#include "Libs/Math/Vector3.h"

namespace collision
{
  class CBoundingBox
  {
  public:
    CBoundingBox() : m_v3Min(math::CVector3::Zero), m_v3Max(math::CVector3::Zero) {}
    CBoundingBox(const math::CVector3& _v3Min, const math::CVector3& _v3Max) : m_v3Min(_v3Min), m_v3Max(_v3Max) {}
    ~CBoundingBox() {}

    inline void SetMin(const math::CVector3& _v3Min) { m_v3Min = _v3Min; }
    inline const math::CVector3& GetMin() const { return m_v3Min; }
    inline void SetMax(const math::CVector3& _v3Max) { m_v3Max = _v3Max; }
    inline const math::CVector3& GetMax() const { return m_v3Max; }

    void GetExtents(math::CVector3 lstExtents[8]) const;
    inline math::CVector3 GetCenter() const { return math::CVector3(m_v3Max + m_v3Min) * 0.5f; }
    inline math::CVector3 GetHalfSize() const { return math::CVector3(m_v3Max - m_v3Min) * 0.5f; }
    inline math::CVector3 GetSize() const { return math::CVector3(m_v3Max - m_v3Min); }

    void DrawDebug();

  private:
    math::CVector3 m_v3Min;
    math::CVector3 m_v3Max;
  };
}

