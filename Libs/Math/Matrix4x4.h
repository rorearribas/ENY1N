#pragma once
#include "Vector3.h"

namespace math
{
  class CMatrix4x4
  {
  private:
    static constexpr int s_iColumnSize = 4;
    static constexpr int s_iRowSize = 4;

    // column-major (DirectX/HLSL)
    typedef float TMatrix16[s_iColumnSize * s_iRowSize];
    TMatrix16 m; // column-major style

  public:
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4() = default;
    CMatrix4x4(const CMatrix4x4& _mMatrix)
    {
      std::memcpy(m, _mMatrix.m, sizeof(TMatrix16));
    }
    CMatrix4x4
    (
      float _m11, float _m12, float _m13, float _m14,
      float _m21, float _m22, float _m23, float _m24,
      float _m31, float _m32, float _m33, float _m34,
      float _m41, float _m42, float _m43, float _m44
    );

    CMatrix4x4 operator*(const CMatrix4x4& _Other) const;
    inline CMatrix4x4& operator=(const CMatrix4x4& _Other)
    {
      if (this != &_Other)
      {
        std::memcpy(m, _Other.m, sizeof(float) * (s_iColumnSize * s_iRowSize));
      }
      return *this;
    }
    inline math::CVector3 operator*(const math::CVector3& _v3Other) const
    {
      float fX = _v3Other.x * m[0] + _v3Other.y * m[4] + _v3Other.z * m[8] + m[12];
      float fY = _v3Other.x * m[1] + _v3Other.y * m[5] + _v3Other.z * m[9] + m[13];
      float fZ = _v3Other.x * m[2] + _v3Other.y * m[6] + _v3Other.z * m[10] + m[14];
      return math::CVector3(fX, fY, fZ);
    }

    inline operator const float* () const { return &m[0]; }
    inline operator float*() { return &m[0]; }

    inline float operator[] (int _iPos) const { return m[_iPos]; }
    inline float& operator[] (int _iPos) { return m[_iPos]; }

    static CMatrix4x4 LookAt(const CVector3& _v3Pos, const CVector3& _vTarget, const CVector3& _vUp);
    static CMatrix4x4 RotationAxis(const CVector3& _v3Axis, float _fAngle);
    static CMatrix4x4 AlignMatrix(const CVector3& _v3Current, const CVector3& _v3Target);

    static CMatrix4x4 Invert(const CMatrix4x4& _mMatrix);
    void Invert();
    static CMatrix4x4 Transpose(const CMatrix4x4& _mMatrix);
    void Transpose();

    static CMatrix4x4 CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar);
    static CMatrix4x4 CreateOrtographicMatrix(float _fWidth, float _fHeight, float _fNear, float _fFar);

    static CMatrix4x4 Translate(const CVector3& _v3Translate);
    void SetTranslate(const CVector3& _v3Translate);
    math::CVector3 GetTranslate() const;

    static CMatrix4x4 Scale(const CVector3& _v3Scale);
    math::CVector3 GetScale() const;
    const bool HasScaleUniform() const;

    static CMatrix4x4 Rotation(const CVector3& _v3Rot);
    math::CVector3 GetRotation() const;
  };
}