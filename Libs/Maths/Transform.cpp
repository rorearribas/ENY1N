#include "Transform.h"

namespace maths
{
  // ------------------------------------
  void CTransform::SetPosition(const maths::CVector3& _v3Pos)
  {
    m_vPosition = _v3Pos;
    UpdateTranslateMatrix();
  }
  // ------------------------------------
  void CTransform::SetScale(const maths::CVector3& _v3Scale)
  {
    m_vScale = _v3Scale;
    UpdateScaleMatrix();
  }
  // ------------------------------------
  void CTransform::SetRotation(const maths::CVector3& _v3Rotation)
  {
    m_vRotate = _v3Rotation;
    UpdateRotateMatrix();
  }
  // ------------------------------------
  void CTransform::UpdateTranslateMatrix()
  {
    m_mTranslate = maths::CMatrix4x4::Translate(m_vPosition);
  }
  // ------------------------------------
  void CTransform::UpdateRotateMatrix()
  {
    m_mRotate = maths::CMatrix4x4::Rotation(m_vRotate);
  }
  // ------------------------------------
  void CTransform::UpdateScaleMatrix()
  {
    m_mScale = maths::CMatrix4x4::Scale(m_vScale);
  }
}