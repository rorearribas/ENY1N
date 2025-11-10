#pragma once
#include "Matrix4x4.h"

namespace math
{
  class CTransform
  {
  public:
    CTransform() {}
    ~CTransform() {}

    math::CMatrix4x4 CreateTransform() const;

    void SetPosition(const math::CVector3& _v3Pos);
    inline const math::CVector3& GetPosition() const { return m_v3Pos; }
    void SetRotation(const math::CVector3& _v3Rotation);
    inline const math::CVector3& GetRotation() const { return m_v3Rot; }
    void SetScale(const math::CVector3& _v3Scale);
    inline const math::CVector3& GetScale() const { return m_v3Scale; }

  private:
    math::CVector3 m_v3Pos = math::CVector3::Zero;
    math::CVector3 m_v3Rot = math::CVector3::Zero;
    math::CVector3 m_v3Scale = math::CVector3::One;
  };
}
