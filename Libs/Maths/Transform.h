#pragma once
#include "Matrix4x4.h"

namespace maths
{
  class CTransform
  {
  public:
    CTransform() {}
    ~CTransform() {}

    maths::CMatrix4x4 ComputeModelMatrix() { return m_mTranslate * (m_mScale * m_mRotate); }

    void SetPosition(const maths::CVector3& _v3Pos);
    void MovePosition(const maths::CVector3& _v3Delta);
    const maths::CVector3& GetPosition() const { return m_vPosition; }

    void SetRotation(const maths::CVector3& _v3Rotation);
    void AddRotation(const maths::CVector3& _v3Delta);
    const maths::CVector3& GetRotation() const { return m_vRotate; }

    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const { return m_vScale; }

  private:
    void UpdateRotateMatrix();
    void UpdateTranslateMatrix();
    void UpdateScaleMatrix();

    maths::CVector3 m_vPosition = maths::CVector3::Zero;
    maths::CVector3 m_vRotate = maths::CVector3::Zero;
    maths::CVector3 m_vScale = maths::CVector3::Zero;

    maths::CMatrix4x4 m_mTranslate = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 m_mRotate = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 m_mScale = maths::CMatrix4x4::Identity;
  };
}
