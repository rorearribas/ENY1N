#pragma once
#include "Matrix4x4.h"

namespace math
{
  class CTransform
  {
  public:
    CTransform() = default;
    CTransform(const math::CMatrix4x4& _mMatrix) : m_mMatrix(_mMatrix) {}
    ~CTransform() {}

    inline const math::CMatrix4x4& GetMatrix() const { return m_mMatrix; }
    inline void SetMatrix(const math::CMatrix4x4& _mMatrix) { m_mMatrix = _mMatrix; }

    void SetPos(const math::CVector3& _v3Pos);
    inline math::CVector3 GetPos() const { return m_mMatrix.GetTranslate(); }
    void SetRot(const math::CVector3& _v3Rot);
    inline math::CVector3 GetRot() const { return m_mMatrix.GetRotation(); }
    void SetScl(const math::CVector3& _v3Scl);
    inline math::CVector3 GetScl() const { return m_mMatrix.GetScale(); }

  private:
    math::CMatrix4x4 m_mMatrix = math::CMatrix4x4::Identity;
  };
}
