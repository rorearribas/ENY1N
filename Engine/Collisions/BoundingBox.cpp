#include "BoundingBox.h"
#include "Engine/Engine.h"

namespace collision
{
  // ------------------------------------
  void CBoundingBox::GetExtents(math::CVector3 lstExtents[8]) const
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
  void CBoundingBox::DrawDebug()
  {
    // Draw cube
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawCube(GetCenter(), GetSize(), math::CVector3::Zero, math::CVector3::Right, render::ERenderMode::WIREFRAME);
  }
}