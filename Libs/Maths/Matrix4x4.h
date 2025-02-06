#pragma once
#include <array>
#include "Vector3.h"

namespace maths
{
  class CMatrix4x4
  {
  public:
    static constexpr int s_iMatrixSize = 16;
    typedef float TMatrix4x4[s_iMatrixSize];
    TMatrix4x4 m;

  public:
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4() = default;

    CMatrix4x4(const float _mMatrix[s_iMatrixSize])
    {
      std::memcpy(m, _mMatrix, sizeof(float) * s_iMatrixSize);
    }

    CMatrix4x4
    (
      float _m11, float _m12, float _m13, float _m14,
      float _m21, float _m22, float _m23, float _m24,
      float _m31, float _m32, float _m33, float _m34,
      float _m41, float _m42, float _m43, float _m44
    );

    maths::CVector3 operator*(const CVector3& _v3) const
    {
      float x = _v3.X * m[0] + _v3.Y * m[4] + _v3.Z * m[8] + m[12];
      float y = _v3.X * m[1] + _v3.Y * m[5] + _v3.Z * m[9] + m[13];
      float z = _v3.X * m[2] + _v3.Y * m[6] + _v3.Z * m[10] + m[14];
      return CVector3(x, y, z);
    }

    CMatrix4x4 operator*(const CMatrix4x4& _mMatrix) const
    {
      CMatrix4x4 mMatrix = CMatrix4x4::Identity;
      for (int i = 0; i < 4; ++i)
      {
        for (int j = 0; j < 4; ++j)
        {
          mMatrix.m[i * 4 + j] = m[i * 4 + 0] * _mMatrix.m[0 * 4 + j] +
            m[i * 4 + 1] * _mMatrix.m[1 * 4 + j] +
            m[i * 4 + 2] * _mMatrix.m[2 * 4 + j] +
            m[i * 4 + 3] * _mMatrix.m[3 * 4 + j];
        }
      }
      return mMatrix;
    }

    CMatrix4x4& operator=(const CMatrix4x4& _other)
    {
      if (this != &_other) 
      {
        std::memcpy(m, _other.m, sizeof(float) * s_iMatrixSize);
      }
      return *this;
    }

    static CMatrix4x4 CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar);
    static CMatrix4x4 LookAt(const CVector3& _vEye, const CVector3& _vTarget, const CVector3& _vUp);
    static CMatrix4x4 Rotation(const CVector3& _vRot);
    static CMatrix4x4 Translate(const CVector3& _vTranslate);
    static CMatrix4x4 Scale(const CVector3& _vScale);
    static CMatrix4x4 Transpose(const CMatrix4x4& matrix);
  };
}