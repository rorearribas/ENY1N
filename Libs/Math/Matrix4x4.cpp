#include "Matrix4x4.h"
#include "Math.h"

namespace math
{
  const CMatrix4x4 CMatrix4x4::Identity =
  {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  // ------------------------------------
  const CMatrix4x4 CMatrix4x4::Zero =
  {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };
  // ------------------------------------
  CMatrix4x4::CMatrix4x4
  (
    float _m11, float _m12, float _m13, float _m14,
    float _m21, float _m22, float _m23, float _m24,
    float _m31, float _m32, float _m33, float _m34,
    float _m41, float _m42, float _m43, float _m44
  )
  {
    m[0][0] = _m11; m[0][1] = _m12; m[0][2] = _m13; m[0][3] = _m14;
    m[1][0] = _m21; m[1][1] = _m22; m[1][2] = _m23; m[1][3] = _m24;
    m[2][0] = _m31; m[2][1] = _m32; m[2][2] = _m33; m[2][3] = _m34;
    m[3][0] = _m41; m[3][1] = _m42; m[3][2] = _m43; m[3][3] = _m44;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar)
  {
    // Calculate tangent
    float fRadians = math::DegreesToRadians(_fFov);
    float fTan = tanf(_fFov / 2.0f);

    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0][0] = 1.0f / (_fAspectRatio * fTan);
    mMatrix.m[1][1] = 1.0f / fTan;
    mMatrix.m[2][2] = _fFar / (_fFar - _fNear);
    mMatrix.m[2][3] = 1.0f;
    mMatrix.m[3][2] = -(_fFar * _fNear) / (_fFar - _fNear);
    mMatrix.m[3][3] = 0.0f;

    return mMatrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _v3Pos, const CVector3& _vTarget, const CVector3& _vUp)
  {
    // Z Axis
    CVector3 v3Offset = _vTarget - _v3Pos;
    CVector3 v3Dir = math::CVector3::Normalize(v3Offset);

    // X axis
    CVector3 v3Cross = _vUp.Cross(v3Dir);
    CVector3 xAxis = math::CVector3::Normalize(v3Cross);

    // Y axis
    CVector3 yAxis = v3Dir.Cross(xAxis);

    CMatrix4x4 mMatrix = CMatrix4x4::Identity;

    mMatrix.m[0][0] = xAxis.X;
    mMatrix.m[1][0] = xAxis.Y;
    mMatrix.m[2][0] = xAxis.Z;
    mMatrix.m[3][0] = -xAxis.Dot(_v3Pos);

    mMatrix.m[0][1] = yAxis.X;
    mMatrix.m[1][1] = yAxis.Y;
    mMatrix.m[2][1] = yAxis.Z;
    mMatrix.m[3][1] = -yAxis.Dot(_v3Pos);

    mMatrix.m[0][2] = v3Dir.X;
    mMatrix.m[1][2] = v3Dir.Y;
    mMatrix.m[2][2] = v3Dir.Z;
    mMatrix.m[3][2] = -v3Dir.Dot(_v3Pos);

    mMatrix.m[3][3] = 1.0f;

    return mMatrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Translate(const CVector3& _vTranslate)
  {
    CMatrix4x4 Matrix = CMatrix4x4::Identity;
    Matrix.m[0][3] = _vTranslate.X;
    Matrix.m[1][3] = _vTranslate.Y;
    Matrix.m[2][3] = _vTranslate.Z;
    return Matrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Scale(const CVector3& _vScale)
  {
    CMatrix4x4 Matrix = CMatrix4x4::Identity;
    Matrix.m[0][0] = _vScale.X;
    Matrix.m[1][1] = _vScale.Y;
    Matrix.m[2][2] = _vScale.Z;
    return Matrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Rotation(const CVector3& _vRot)
  {
    float fPitch = math::DegreesToRadians(_vRot.X);
    float fYaw = math::DegreesToRadians(_vRot.Y);
    float fRoll = math::DegreesToRadians(_vRot.Z);

    CMatrix4x4 mRollMatrix = CMatrix4x4::Identity;
    mRollMatrix.m[0][0] = cos(fRoll);
    mRollMatrix.m[0][1] = sin(fRoll);
    mRollMatrix.m[1][0] = -sin(fRoll);
    mRollMatrix.m[1][1] = cos(fRoll);

    CMatrix4x4 mPitchMatrix = CMatrix4x4::Identity;
    mPitchMatrix.m[1][1] = cos(fPitch);
    mPitchMatrix.m[1][2] = sin(fPitch);
    mPitchMatrix.m[2][1] = -sin(fPitch);
    mPitchMatrix.m[2][2] = cos(fPitch);

    CMatrix4x4 mYawMatrix = CMatrix4x4::Identity;
    mYawMatrix.m[0][0] = cos(fYaw);
    mYawMatrix.m[0][2] = -sin(fYaw);
    mYawMatrix.m[2][0] = sin(fYaw);
    mYawMatrix.m[2][2] = cos(fYaw);

    return mRollMatrix * mPitchMatrix * mYawMatrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        mMatrix.m[i][j] = _mMatrix.m[j][i];
      }
    }
    return mMatrix;
  }
}