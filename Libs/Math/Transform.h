#pragma once
#include "Matrix4x4.h"

namespace math
{
  class CTransform
  {
  public:
    CTransform() = default;
    CTransform(const math::CMatrix4x4& _mMatrix) : m_mTransform(_mMatrix) {}
    ~CTransform() {}

    inline const math::CMatrix4x4& GetMatrix() const { return m_mTransform; }
    inline void SetMatrix(const math::CMatrix4x4& _mMatrix) { m_mTransform = _mMatrix; }

    void SetPosition(const math::CVector3& _v3Pos);
    inline math::CVector3 GetTranslation() const { return m_mTransform.GetTranslate(); }
    void SetRotation(const math::CVector3& _v3Rotation);
    inline math::CVector3 GetRotation() const { return m_mTransform.GetRotation(); }
    void SetScale(const math::CVector3& _v3Scale);
    inline math::CVector3 GetScale() const { return m_mTransform.GetScale(); }

  private:
    math::CMatrix4x4 m_mTransform = math::CMatrix4x4::Identity;
  };
}
