#pragma once
#include <array>
#include <cstring>
#include "Vector3.h"

namespace math
{
  class CMatrix4x4
  {
  private:
    static constexpr int s_iRowSize = 4;
    static constexpr int s_iColumnSize = 4;
    typedef float TMatrix4x4[s_iRowSize][s_iColumnSize];
    TMatrix4x4 m;

  public:
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4() = default;

    CMatrix4x4(const float _mMatrix[s_iRowSize][s_iColumnSize])
    {
      std::memcpy(m, _mMatrix, sizeof(float) * (s_iRowSize * s_iColumnSize));
    }

    CMatrix4x4
    (
      float _m11, float _m12, float _m13, float _m14,
      float _m21, float _m22, float _m23, float _m24,
      float _m31, float _m32, float _m33, float _m34,
      float _m41, float _m42, float _m43, float _m44
    );

    math::CVector3 operator*(const CVector3& _v3) const
    {
      float fX = _v3.X * m[0][0] + _v3.Y * m[1][0] + _v3.Z * m[2][0] + m[3][0];
      float fY = _v3.X * m[0][1] + _v3.Y * m[1][1] + _v3.Z * m[2][1] + m[3][1];
      float fZ = _v3.X * m[0][2] + _v3.Y * m[1][2] + _v3.Z * m[2][2] + m[3][2];
      return CVector3(fX, fY, fZ);
    }

    CMatrix4x4 operator*(const CMatrix4x4& rhs) const
    {
      CMatrix4x4 result;
      for (int row = 0; row < 4; ++row)
      {
        for (int col = 0; col < 4; ++col)
        {
          result.m[row][col] = 0.0f;
          for (int k = 0; k < 4; ++k)
          {
            result.m[row][col] += m[row][k] * rhs.m[k][col];
          }
        }
      }
      return result;
    }

    CMatrix4x4& operator=(const CMatrix4x4& _other)
    {
      if (this != &_other)
      {
        std::memcpy(m, _other.m, sizeof(float) * (s_iRowSize * s_iColumnSize));
      }
      return *this;
    }

    const float* operator()() const { return &m[0][0]; }
    float* operator()() { return &m[0][0]; }

    float& At(size_t row, size_t col) { return m[row][col]; }
    const float& At(size_t row, size_t col) const { return m[row][col]; }

    static CMatrix4x4 CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar);
    static CMatrix4x4 LookAt(const CVector3& _v3Pos, const CVector3& _vTarget, const CVector3& _vUp);
    static CMatrix4x4 Rotation(const CVector3& _vRot);
    static CMatrix4x4 RotationAxis(const CVector3& _v3Axis, float angleRadians);
    static CMatrix4x4 Translate(const CVector3& _vTranslate);
    static CMatrix4x4 Scale(const CVector3& _vScale);
    static CMatrix4x4 Transpose(const CMatrix4x4& matrix);
  };
}