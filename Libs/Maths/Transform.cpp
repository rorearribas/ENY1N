#include "Transform.h"

namespace maths
{
  const maths::CMatrix4x4 CTransform::ComputeModelMatrix() const
  {
    return maths::CMatrix4x4::Translate(m_v3Pos) * (maths::CMatrix4x4::Scale(m_v3Scale) * maths::CMatrix4x4::Rotation(m_v3Rot));
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
    m_v3Rot = _v3Rotation;
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