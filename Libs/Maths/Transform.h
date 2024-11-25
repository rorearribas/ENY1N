#pragma once
#include "Matrix4x4.h"

namespace maths
{
  class CTransform
  {
  public:
    CTransform() {}
    ~CTransform() {}

    const maths::CMatrix4x4 ComputeModelMatrix() const;

    void SetPosition(const maths::CVector3& _v3Pos);
    const maths::CVector3& GetPosition() const;
    void SetRotation(const maths::CVector3& _v3Rotation);
    const maths::CVector3& GetRotation() const;
    void SetScale(const maths::CVector3& _v3Scale);
    const maths::CVector3& GetScale() const;

  private:
    maths::CVector3 m_v3Pos = maths::CVector3::Zero;
    maths::CVector3 m_v3Rot = maths::CVector3::Zero;
    maths::CVector3 m_v3Scale = maths::CVector3::One;
  };
}
