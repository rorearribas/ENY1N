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
    auto oClampFunc = [](float _fAngle)
    {
      _fAngle = fmod(_fAngle, 360.0f);
      _fAngle = (_fAngle > 180.0f) ? (_fAngle - 360.0f) : (_fAngle < -180.0f ? (_fAngle + 360.0f) : _fAngle);
      return _fAngle;
    };
    m_v3Rot.X = oClampFunc(_v3Rotation.X);
    m_v3Rot.Y = oClampFunc(_v3Rotation.Y);
    m_v3Rot.Z = oClampFunc(_v3Rotation.Z);
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