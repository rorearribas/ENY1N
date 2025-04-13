#include "Transform.h"

namespace maths
{
  namespace internal_transform
  {
    const float fMaxDegrees = 360.0f;
    static float ComputeValidAngle(float fAngle)
    {
      fAngle = fmod(fAngle, fMaxDegrees);
      fAngle = (fAngle > fMaxDegrees / 2.0f) ? (fAngle - fMaxDegrees) : (fAngle < -(fMaxDegrees / 2.0f) ? (fAngle + fMaxDegrees) : fAngle);
      return fAngle;
    }
  }

  const maths::CMatrix4x4 CTransform::ComputeModelMatrix() const
  {
    return maths::CMatrix4x4::Translate(m_v3Pos) * maths::CMatrix4x4::Scale(m_v3Scale) * maths::CMatrix4x4::Rotation(m_v3Rot);
  }
  // ------------------------------------
  void CTransform::SetPosition(const maths::CVector3& _v3Pos)
  {
    m_v3Pos = _v3Pos;
  }
  // ------------------------------------
  const maths::CVector3& CTransform::GetPosition() const
  {
    return m_v3Pos;
  }
  // ------------------------------------
  void CTransform::SetRotation(const maths::CVector3& _v3Rotation)
  {
    m_v3Rot.X = internal_transform::ComputeValidAngle(_v3Rotation.X);
    m_v3Rot.Y = internal_transform::ComputeValidAngle(_v3Rotation.Y);
    m_v3Rot.Z = internal_transform::ComputeValidAngle(_v3Rotation.Z);
  }
  // ------------------------------------
  const maths::CVector3& CTransform::GetRotation() const
  {
    return m_v3Rot;
  }
  // ------------------------------------
  void CTransform::SetScale(const maths::CVector3& _v3Scale)
  {
    m_v3Scale = _v3Scale;
  }
  // ------------------------------------
  const maths::CVector3& CTransform::GetScale() const
  {
    return m_v3Scale;
  }
}