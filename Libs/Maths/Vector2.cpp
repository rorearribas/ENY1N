#include "Vector2.h"
#include <cmath>

namespace maths
{
  maths::CVector2 CVector2::vEMPTY(0.0f, 0.0f);
  maths::CVector2 CVector2::vRight(1.0f, 0.0f);
  maths::CVector2 CVector2::vUp(0.0f, 1.0f);

  // ------------------------------------
  float CVector2::DotProduct(const CVector2& _v2) const
  {
    return (m_x * _v2.m_x) + (m_y * _v2.m_y);
  }
  // ------------------------------------
  float CVector2::CrossProduct(const CVector2& _v2) const
  {
    return m_x * _v2.m_y - m_y * _v2.m_x;
  }
  // ------------------------------------
  void CVector2::Normalize()
  {
    float fMagnitude = sqrt((m_x * m_x) + (m_y * m_y));
    if (fMagnitude > 0) 
    {
      m_x /= fMagnitude;
      m_y /= fMagnitude;
    }
  }

}

