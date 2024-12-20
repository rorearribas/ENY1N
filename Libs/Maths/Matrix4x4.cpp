#include "Matrix4x4.h"

namespace maths
{
  const CMatrix4x4 CMatrix4x4::Identity = 
  {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  const CMatrix4x4 CMatrix4x4::Zero =
  {
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
  };
  // ------------------------------------
  CMatrix4x4::CMatrix4x4(const float _mMatrix[s_iMatrixSize][s_iMatrixSize])
  {
    for (int iRowIdx = 0; iRowIdx < s_iMatrixSize; iRowIdx++)
    {
      for (int iColumnIdx = 0; iColumnIdx < s_iMatrixSize; iColumnIdx++)
      {
        m[iRowIdx][iColumnIdx] = _mMatrix[iRowIdx][iColumnIdx];
      }
    }
  }
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
  maths::CMatrix4x4 CMatrix4x4::CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar)
  {
    CMatrix4x4 matrix = CMatrix4x4::Identity;
    float fTan = tanf(_fFov / 2.0f);

    matrix.m[0][0] = 1.0f / (_fAspectRatio * fTan);
    matrix.m[1][1] = 1.0f / fTan;

    matrix.m[2][2] = _fFar / (_fFar - _fNear);
    matrix.m[2][3] = 1.0f;

    matrix.m[3][2] = -(_fFar * _fNear) / (_fFar - _fNear);
    matrix.m[3][3] = 0.0f;

    return matrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _vEye, const CVector3& _vTarget, const CVector3& _vUp)
  {
    // Z Axis
    CVector3 vDir = _vTarget - _vEye;
    CVector3 zAxis = vDir.Normalized();

    // X axis
    CVector3 vCross = _vUp.CrossProduct(zAxis);
    CVector3 xAxis = vCross.Normalized();

    // Y axis
    CVector3 yAxis = zAxis.CrossProduct(xAxis);

    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0][0] = xAxis.X; mMatrix.m[0][1] = yAxis.X; mMatrix.m[0][2] = zAxis.X; mMatrix.m[0][3] = 0.0f;
    mMatrix.m[1][0] = xAxis.Y; mMatrix.m[1][1] = yAxis.Y; mMatrix.m[1][2] = zAxis.Y; mMatrix.m[1][3] = 0.0f;
    mMatrix.m[2][0] = xAxis.Z; mMatrix.m[2][1] = yAxis.Z; mMatrix.m[2][2] = zAxis.Z; mMatrix.m[2][3] = 0.0f;
    mMatrix.m[3][0] = -xAxis.DotProduct(_vEye); mMatrix.m[3][1] = -yAxis.DotProduct(_vEye); mMatrix.m[3][2] = -zAxis.DotProduct(_vEye); mMatrix.m[3][3] = 1.0f;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Translate(const CVector3& _vTranslate)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0][3] = _vTranslate.X;
    mMatrix.m[1][3] = _vTranslate.Y;
    mMatrix.m[2][3] = _vTranslate.Z;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Scale(const CVector3& _vScale)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.m[0][0] = _vScale.X;
    mMatrix.m[1][1] = _vScale.Y;
    mMatrix.m[2][2] = _vScale.Z;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Rotation(const CVector3& _vRot)
  {
    CMatrix4x4 mPitchMatrix = CMatrix4x4::Identity;
    mPitchMatrix.m[1][1] = (float)cos(_vRot.X);
    mPitchMatrix.m[1][2] = (float)sin(_vRot.X);
    mPitchMatrix.m[2][1] = -(float)sin(_vRot.X);
    mPitchMatrix.m[2][2] = (float)cos(_vRot.X);

    CMatrix4x4 mYawMatrix = CMatrix4x4::Identity;
    mYawMatrix.m[0][0] = (float)cos(_vRot.Y);
    mYawMatrix.m[0][2] = -(float)sin(_vRot.Y);
    mYawMatrix.m[2][0] = (float)sin(_vRot.Y);
    mYawMatrix.m[2][2] = (float)cos(_vRot.Y);

    CMatrix4x4 mRollMatrix = CMatrix4x4::Identity;
    mRollMatrix.m[0][0] = (float)cos(_vRot.Z);
    mRollMatrix.m[0][1] = (float)sin(_vRot.Z);
    mRollMatrix.m[1][0] = -(float)sin(_vRot.Z);
    mRollMatrix.m[1][1] = (float)cos(_vRot.Z);

    return mPitchMatrix * mYawMatrix * mRollMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        mMatrix.m[i][j] = _mMatrix.m[j][i];
      }
    }
    return mMatrix;
  }
}