#include "Transform.h"
#include "Math.h"

namespace math
{
  // ------------------------------------
  math::CMatrix4x4 CTransform::GetMatrix() const
  {
    return math::CMatrix4x4::Translate(m_v3Pos) * math::CMatrix4x4::Rotation(m_v3Rot) * math::CMatrix4x4::Scale(m_v3Scale);
  }
  // ------------------------------------
  void CTransform::SetPosition(const math::CVector3& _v3Pos)
  {
    m_v3Pos = _v3Pos;
  }
  // ------------------------------------
  void CTransform::SetRotation(const math::CVector3& _v3Rotation)
  {
    m_v3Rot.x = math::CalculateEulerAngle(_v3Rotation.x);
    m_v3Rot.y = math::CalculateEulerAngle(_v3Rotation.y);
    m_v3Rot.z = math::CalculateEulerAngle(_v3Rotation.z);
  }
  // ------------------------------------
  void CTransform::SetScale(const math::CVector3& _v3Scale)
  {
    m_v3Scale = _v3Scale;
  }
}