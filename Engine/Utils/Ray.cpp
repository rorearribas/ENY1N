#include "Ray.h"
#include "Engine/Engine.h"

namespace physics
{
  math::CVector3 CRay::CalculatePoint(float _fDistance) const
  {
    return m_v3Origin + (m_v3Dir * _fDistance);
  }
  // ------------------------------------
  math::CVector3 CRay::CalculatePoint(const CRay& _oRay, float _fDist)
  {
    return _oRay.CalculatePoint(_fDist);
  }
  // ------------------------------------
  void CRay::DrawRay(float _fDist, const math::CVector3& _v3Color)
  {
    math::CVector3 v3TargetPos = CalculatePoint(_fDist);
    engine::CEngine::GetInstance()->DrawLine(m_v3Origin, v3TargetPos, _v3Color);
  }
}

