#pragma once
#include <array>
#include "Vector3.h"

namespace maths
{
  class __declspec(align(16)) CMatrix4x4
  {
    static constexpr int s_iMatrixSize = 4;

  public:
    typedef float TMatrix4x4[s_iMatrixSize][s_iMatrixSize];
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4() = default;
    CMatrix4x4(const float _mMatrix[s_iMatrixSize][s_iMatrixSize]);
    CMatrix4x4
    (
      float _m11, float _m12, float _m13, float _m14,
      float _m21, float _m22, float _m23, float _m24,
      float _m31, float _m32, float _m33, float _m34,
      float _m41, float _m42, float _m43, float _m44
    );

    CMatrix4x4 operator*(const CMatrix4x4& mat) const 
    {
      CMatrix4x4 result;
      for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
          result.mMat[i][j] = mMat[i][0] * mat.mMat[0][j] +
            mMat[i][1] * mat.mMat[1][j] +
            mMat[i][2] * mat.mMat[2][j] +
            mMat[i][3] * mat.mMat[3][j];
        }
      }
      return result;
    }

    maths::CVector3 operator*(const CVector3& vec) const 
    {
      float x = vec.X * mMat[0][0] + vec.Y * mMat[1][0] + vec.Z * mMat[2][0] + mMat[3][0];
      float y = vec.X * mMat[0][1] + vec.Y * mMat[1][1] + vec.Z * mMat[2][1] + mMat[3][1];
      float z = vec.X * mMat[0][2] + vec.Y * mMat[1][2] + vec.Z * mMat[2][2] + mMat[3][2];
      return CVector3(x, y, z);
    }

    CMatrix4x4& operator=(const CMatrix4x4& other) 
    {
      if (this != &other) {  // Evita la autoasignación
        for (int i = 0; i < 4; ++i) {
          for (int j = 0; j < 4; ++j) {
            mMat[i][j] = other.mMat[i][j];
          }
        }
      }
      return *this;  // Devuelve la referencia a la matriz actual
    }

    static CMatrix4x4 CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar);
    static CMatrix4x4 LookAt(const CVector3& _vEye, const CVector3& _vTarget, const CVector3& _vUp);
    static CMatrix4x4 Rotation(float pitch, float yaw, float roll);
    static CMatrix4x4 Transpose(const CMatrix4x4& matrix);

  private:
    TMatrix4x4 mMat;
  };
}



