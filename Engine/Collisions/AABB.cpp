#include "AABB.h"
#include "Engine/Engine.h"

namespace collision
{
  // ------------------------------------
  void CAABB::GetExtents(math::CVector3 lstExtents[8]) const
  {
    lstExtents[0] = m_v3Min;
    {
      lstExtents[1] = math::CVector3(m_v3Max.x, m_v3Min.y, m_v3Min.z);
      lstExtents[2] = math::CVector3(m_v3Min.x, m_v3Max.y, m_v3Min.z);
      lstExtents[3] = math::CVector3(m_v3Max.x, m_v3Max.y, m_v3Min.z);
      lstExtents[4] = math::CVector3(m_v3Min.x, m_v3Min.y, m_v3Max.z);
      lstExtents[5] = math::CVector3(m_v3Max.x, m_v3Min.y, m_v3Max.z);
      lstExtents[6] = math::CVector3(m_v3Min.x, m_v3Max.y, m_v3Max.z);
    }
    lstExtents[7] = m_v3Max;
  }
  // ------------------------------------
  void CAABB::DrawDebug(math::CVector3 _v3Color) const
  {
    // Draw cube
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawCube(GetCenter(), math::CVector3::Zero, GetSize(), _v3Color, render::ERenderMode::WIREFRAME);
  }
  // ------------------------------------
  void ComputeWorldAABB(const CAABB& _rLocalAABB, const math::CTransform& _mTransform, CAABB& _rWorldAABB_)
  {
    const int iExtentsCount = 8;
    math::CVector3 lstExtents[iExtentsCount];
    _rLocalAABB.GetExtents(lstExtents);

    // Compute world AABB
    math::CVector3 v3Min(FLT_MAX, FLT_MAX, FLT_MAX);
    math::CVector3 v3Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (uint32_t uI = 0; uI < iExtentsCount; uI++)
    {
      // Calculate extent pos
      math::CVector3 v3Extent = _mTransform.GetMatrix() * lstExtents[uI];

      // Calculate Min
      v3Min.x = math::Min(v3Min.x, v3Extent.x);
      v3Min.y = math::Min(v3Min.y, v3Extent.y);
      v3Min.z = math::Min(v3Min.z, v3Extent.z);

      // Calculate Max
      v3Max.x = math::Max(v3Max.x, v3Extent.x);
      v3Max.y = math::Max(v3Max.y, v3Extent.y);
      v3Max.z = math::Max(v3Max.z, v3Extent.z);
    }

    // Set World AABB
    _rWorldAABB_.SetMin(v3Min);
    _rWorldAABB_.SetMax(v3Max);
  }

}