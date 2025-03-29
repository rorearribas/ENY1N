#include "Matrix4x4.h"
#include "Maths.h"

namespace maths
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
    m[0] = _m11; m[1] = _m12; m[2] = _m13; m[3] = _m14;
    m[4] = _m21; m[5] = _m22; m[6] = _m23; m[7] = _m24;
    m[8] = _m31; m[9] = _m32; m[10] = _m33; m[11] = _m34;
    m[12] = _m41; m[13] = _m42; m[14] = _m43; m[15] = _m44;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar)
  {
    CMatrix4x4 matrix = CMatrix4x4::Identity;
    float fTan = tanf(_fFov / 2.0f);

    matrix.m[0] = 1.0f / (_fAspectRatio * fTan);
    matrix.m[5] = 1.0f / fTan;
    matrix.m[10] = _fFar / (_fFar - _fNear);
    matrix.m[11] = 1.0f;
    matrix.m[14] = -(_fFar * _fNear) / (_fFar - _fNear);
    matrix.m[15] = 0.0f;

    return matrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _vEye, const CVector3& _vTarget, const CVector3& _vUp)
  {
    // Z Axis
    CVector3 vDir = _vTarget - _vEye;
    CVector3 zAxis = vDir.Normalize();

    // X axis
    CVector3 vCross = _vUp.Cross(zAxis);
    CVector3 xAxis = vCross.Normalize();

    // Y axis
    CVector3 yAxis = zAxis.Cross(xAxis);

    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0] = xAxis.X; mMatrix.m[1] = yAxis.X; mMatrix.m[2] = zAxis.X; mMatrix.m[3] = 0.0f;
    mMatrix.m[4] = xAxis.Y; mMatrix.m[5] = yAxis.Y; mMatrix.m[6] = zAxis.Y; mMatrix.m[7] = 0.0f;
    mMatrix.m[4] = xAxis.Y; mMatrix.m[5] = yAxis.Y; mMatrix.m[6] = zAxis.Y; mMatrix.m[7] = 0.0f;
    mMatrix.m[8] = xAxis.Z; mMatrix.m[9] = yAxis.Z; mMatrix.m[10] = zAxis.Z; mMatrix.m[11] = 0.0f;
    mMatrix.m[12] = -xAxis.Dot(_vEye); mMatrix.m[13] = -yAxis.Dot(_vEye); mMatrix.m[14] = -zAxis.Dot(_vEye); mMatrix.m[15] = 1.0f;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Translate(const CVector3& _vTranslate)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[3] = _vTranslate.X;
    mMatrix.m[7] = _vTranslate.Y;
    mMatrix.m[11] = _vTranslate.Z;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Scale(const CVector3& _vScale)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0] = _vScale.X;
    mMatrix.m[5] = _vScale.Y;
    mMatrix.m[10] = _vScale.Z;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Rotation(const CVector3& _vRot)
  {
    float fPitch = maths::DegreesToRadians(_vRot.X);
    float fYaw = maths::DegreesToRadians(_vRot.Y);
    float fRoll = maths::DegreesToRadians(_vRot.Z);

    CMatrix4x4 mRollMatrix = CMatrix4x4::Identity;
    mRollMatrix.m[0] = cos(fRoll);
    mRollMatrix.m[1] = sin(fRoll);
    mRollMatrix.m[4] = -sin(fRoll);
    mRollMatrix.m[5] = cos(fRoll);

    CMatrix4x4 mPitchMatrix = CMatrix4x4::Identity;
    mPitchMatrix.m[5] = cos(fPitch);
    mPitchMatrix.m[6] = sin(fPitch);
    mPitchMatrix.m[9] = -sin(fPitch);
    mPitchMatrix.m[10] = cos(fPitch);

    CMatrix4x4 mYawMatrix = CMatrix4x4::Identity;
    mYawMatrix.m[0] = cos(fYaw);
    mYawMatrix.m[2] = -sin(fYaw);
    mYawMatrix.m[8] = sin(fYaw);
    mYawMatrix.m[10] = cos(fYaw);

    return mRollMatrix * mPitchMatrix * mYawMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        mMatrix.m[i * 4 + j] = _mMatrix.m[j * 4 + i];
      }
    }
    return mMatrix;
  }
}