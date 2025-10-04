#pragma once
#include <array>
#include <cstring>
#include "Vector3.h"

namespace math
{
  class CMatrix4x4
  {
  private:
    static constexpr int s_iColumnSize = 4;
    static constexpr int s_iRowSize = 4;

    // column-major (DirectX/HLSL)
    typedef float TMatrix4x4[s_iColumnSize][s_iRowSize];
    typedef float TMatrix16[s_iColumnSize * s_iRowSize];
    union
    {
      TMatrix4x4 m4x4; // for debugging purposes only
      TMatrix16 m16; // column-major style
    };

  public:
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4() = default;
    CMatrix4x4(const TMatrix4x4& _mMatrix)
    {
      std::memcpy(m16, _mMatrix, sizeof(TMatrix4x4));
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
        std::memcpy(m16, _Other.m16, sizeof(float) * (s_iColumnSize * s_iRowSize));
      }
      return *this;
    }
    inline math::CVector3 operator*(const math::CVector3& _v3Other) const
    {
      float fX = _v3Other.x * m16[0] + _v3Other.y * m16[4] + _v3Other.z * m16[8] + m16[12];
      float fY = _v3Other.x * m16[1] + _v3Other.y * m16[5] + _v3Other.z * m16[9] + m16[13];
      float fZ = _v3Other.x * m16[2] + _v3Other.y * m16[6] + _v3Other.z * m16[10] + m16[14];
      return math::CVector3(fX, fY, fZ);
    }

    inline const float* operator()() const { return &m16[0]; }
    inline float* operator()() { return &m16[0]; }

    inline float operator[] (int _iPos) const { return m16[_iPos]; }
    inline float& operator[] (int _iPos) { return m16[_iPos]; }

    static CMatrix4x4 LookAt(const CVector3& _v3Pos, const CVector3& _vTarget, const CVector3& _vUp);
    static CMatrix4x4 RotationAxis(const CVector3& _v3Axis, float _fAngle);
    static CMatrix4x4 AlignMatrix(const CVector3& _v3Current, const CVector3& _v3Target);
    static CMatrix4x4 Transpose(const CMatrix4x4& matrix);

    static CMatrix4x4 CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar);
    static CMatrix4x4 CreateOrtographicMatrix(float _fWidth, float _fHeight, float _fNear, float _fFar);

    static CMatrix4x4 Translate(const CVector3& _v3Translate);
    static CMatrix4x4 Scale(const CVector3& _v3Scale);
    static CMatrix4x4 Rotation(const CVector3& _v3Rot);
  };
}