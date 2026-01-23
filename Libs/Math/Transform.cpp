#include "Transform.h"
#include "Math.h"

namespace math
{
  // ------------------------------------
  void CTransform::SetPos(const math::CVector3& _v3Pos)
  {
    // Set pos
    m_mMatrix.SetTranslate(_v3Pos);
  }
  // ------------------------------------
  void CTransform::SetRot(const math::CVector3& _v3Rot)
  {
    // Calculate valid angle!
    math::CVector3 v3Angle = _v3Rot;
    v3Angle.x = CalculateEulerAngle(_v3Rot.x);
    v3Angle.y = CalculateEulerAngle(_v3Rot.y);
    v3Angle.z = CalculateEulerAngle(_v3Rot.z);

    // Update transform
    math::CVector3 v3Translate = m_mMatrix.GetTranslate();
    m_mMatrix = CMatrix4x4::CreateRotation(v3Angle) * CMatrix4x4::CreateScale(m_mMatrix.GetScale());
    m_mMatrix.SetTranslate(v3Translate);
  }
  // ------------------------------------
  void CTransform::SetScl(const math::CVector3& _v3Scl)
  {
    // Update scale
    math::CVector3 v3Translate = m_mMatrix.GetTranslate();
    m_mMatrix = CMatrix4x4::CreateRotation(m_mMatrix.GetRotation()) * CMatrix4x4::CreateScale(_v3Scl);
    m_mMatrix.SetTranslate(v3Translate);
  }
}