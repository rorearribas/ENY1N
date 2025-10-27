#include "BoundingBox.h"
#include "Engine/Engine.h"

namespace collision
{
  // ------------------------------------
  std::vector<math::CVector3> CBoundingBox::GetExtents() const
  {
    std::vector<math::CVector3> vctExtents = 
    {
      math::CVector3(m_v3Min.x, m_v3Min.y, m_v3Min.z),
      math::CVector3(m_v3Max.x, m_v3Min.y, m_v3Min.z),
      math::CVector3(m_v3Min.x, m_v3Max.y, m_v3Min.z),
      math::CVector3(m_v3Max.x, m_v3Max.y, m_v3Min.z),
      math::CVector3(m_v3Min.x, m_v3Min.y, m_v3Max.z),
      math::CVector3(m_v3Max.x, m_v3Min.y, m_v3Max.z),
      math::CVector3(m_v3Min.x, m_v3Max.y, m_v3Max.z),
      math::CVector3(m_v3Max.x, m_v3Max.y, m_v3Max.z)
    };
    return vctExtents;
  }
  // ------------------------------------
  void CBoundingBox::DrawDebug()
  {
    // Draw cube
    engine::CEngine* pEngine = engine::CEngine::GetInstance();
    pEngine->DrawCube(GetCenter(), GetSize(), math::CVector3::Zero, math::CVector3::Right, render::ERenderMode::WIREFRAME);
  }
}