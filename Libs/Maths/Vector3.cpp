#include "Vector3.h"
#include <cmath>

namespace maths
{
  maths::CVector3 CVector3::vEMPTY(0.0f, 0.0f, 0.0f);
  maths::CVector3 CVector3::vForward(0.0f, 0.0f, 1.0f);
  maths::CVector3 CVector3::vRight(1.0f, 0.0f, 0.0f);
  maths::CVector3 CVector3::vUp(0.0f, 1.0f, 0.0f);

  // ------------------------------------
  float CVector3::DotProduct(const CVector3& _v3) const
  {
    return (m_x * _v3.m_x) + (m_y * _v3.m_y) * (m_z * _v3.m_z);
  }
  // ------------------------------------
  CVector3 CVector3::CrossProduct(const CVector3& _v3) const
  {
    return CVector3
    (
      m_y * _v3.m_z - m_z * _v3.m_y, // X
      m_z * _v3.m_x - m_x * _v3.m_z, // Y
      m_x * _v3.m_y - m_y * _v3.m_x  // Z
    );
  }
  // ------------------------------------
  void CVector3::Normalize()
  {
    float fMagnitude = sqrt((m_x * m_x) + (m_y * m_y) + (m_z * m_z));
    if (fMagnitude > 0)
    {
      m_x /= fMagnitude;
      m_y /= fMagnitude;
      m_z /= fMagnitude;
    }
  }
}
