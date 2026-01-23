#include "Transform.h"
#include "Math.h"

namespace math
{
  // ------------------------------------
  void CTransform::SetPosition(const math::CVector3& _v3Pos)
  {
    // Set pos
    m_mTransform.SetTranslate(_v3Pos);
  }
  // ------------------------------------
  void CTransform::SetRotation(const math::CVector3& _v3Rotation)
  {
    // Calculate valid angle!
    math::CVector3 v3Angle = _v3Rotation;
    v3Angle.x = CalculateEulerAngle(_v3Rotation.x);
    v3Angle.y = CalculateEulerAngle(_v3Rotation.y);
    v3Angle.z = CalculateEulerAngle(_v3Rotation.z);

    // Update transform
    math::CVector3 v3Translate = m_mTransform.GetTranslate();
    m_mTransform = CMatrix4x4::CreateRotation(v3Angle) * CMatrix4x4::CreateScale(m_mTransform.GetScale());
    m_mTransform.SetTranslate(v3Translate);
  }
  // ------------------------------------
  void CTransform::SetScale(const math::CVector3& _v3Scale)
  {
    // Update scale
    math::CVector3 v3Translate = m_mTransform.GetTranslate();
    m_mTransform = CMatrix4x4::CreateRotation(m_mTransform.GetRotation()) * CMatrix4x4::CreateScale(_v3Scale);
    m_mTransform.SetTranslate(v3Translate);
  }
}