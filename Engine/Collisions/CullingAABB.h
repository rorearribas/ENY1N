#pragma once
#include "Libs/Math/Vector3.h"

namespace collision
{
  class CCullingAABB
  {
  public:
    CCullingAABB(const math::CVector3& _v3Min, const math::CVector3& _v3Max) :
    m_v3Min(_v3Min), m_v3Max(_v3Max) {}
    ~CCullingAABB() {}

    //bool OnCullingTest(const CCullingAABB& _other);

  private:
    math::CVector3 m_v3Min;
    math::CVector3 m_v3Max;
  };
}


