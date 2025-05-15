#pragma once
#include "Matrix4x4.h"

namespace math
{
  class CTransform
  {
  public:
    CTransform() {}
    ~CTransform() {}

    const math::CMatrix4x4 ComputeModelMatrix() const;

    void SetPosition(const math::CVector3& _v3Pos);
    const math::CVector3& GetPosition() const;
    void SetRotation(const math::CVector3& _v3Rotation);
    const math::CVector3& GetRotation() const;
    void SetScale(const math::CVector3& _v3Scale);
    const math::CVector3& GetScale() const;

  private:
    math::CVector3 m_v3Pos = math::CVector3::Zero;
    math::CVector3 m_v3Rot = math::CVector3::Zero;
    math::CVector3 m_v3Scale = math::CVector3::One;
  };
}
