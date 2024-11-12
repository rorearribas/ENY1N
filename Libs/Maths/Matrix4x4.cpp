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
        mMat[iRowIdx][iColumnIdx] = _mMatrix[iRowIdx][iColumnIdx];
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
    mMat[0][0] = _m11; mMat[0][1] = _m12; mMat[0][2] = _m13; mMat[0][3] = _m14;
    mMat[1][0] = _m21; mMat[1][1] = _m22; mMat[1][2] = _m23; mMat[1][3] = _m24;
    mMat[2][0] = _m31; mMat[2][1] = _m32; mMat[2][2] = _m33; mMat[2][3] = _m34;
    mMat[3][0] = _m41; mMat[3][1] = _m42; mMat[3][2] = _m43; mMat[3][3] = _m44;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar)
  {
    CMatrix4x4 matrix = CMatrix4x4::Identity;
    float fTan = tanf(_fFov / 2.0f);

    matrix.mMat[0][0] = 1.0f / (_fAspectRatio * fTan);
    matrix.mMat[1][1] = 1.0f / fTan;

    matrix.mMat[2][2] = _fFar / (_fFar - _fNear);
    matrix.mMat[2][3] = 1.0f;

    matrix.mMat[3][2] = -(_fFar * _fNear) / (_fFar - _fNear);
    matrix.mMat[3][3] = 0.0f;

    return matrix;

  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _vEye, const CVector3& _vTarget, const CVector3& _vUp)
  {
    CVector3 vDir = _vTarget - _vEye;
    CVector3 zAxis = vDir.Normalized();

    CVector3 vCross = _vUp.CrossProduct(zAxis);
    CVector3 xAxis = vCross.Normalized();

    CVector3 yAxis = zAxis.CrossProduct(xAxis);

    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    mMatrix.mMat[0][0] = xAxis.X; mMatrix.mMat[0][1] = yAxis.X; mMatrix.mMat[0][2] = zAxis.X; mMatrix.mMat[0][3] = 0.0f;
    mMatrix.mMat[1][0] = xAxis.Y; mMatrix.mMat[1][1] = yAxis.Y; mMatrix.mMat[1][2] = zAxis.Y; mMatrix.mMat[1][3] = 0.0f;
    mMatrix.mMat[2][0] = xAxis.Z; mMatrix.mMat[2][1] = yAxis.Z; mMatrix.mMat[2][2] = zAxis.Z; mMatrix.mMat[2][3] = 0.0f;
    mMatrix.mMat[3][0] = -xAxis.DotProduct(_vEye); mMatrix.mMat[3][1] = -yAxis.DotProduct(_vEye); mMatrix.mMat[3][2] = -zAxis.DotProduct(_vEye); mMatrix.mMat[3][3] = 1.0f;
    return mMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Rotation(float pitch, float yaw, float roll)
  {
    CMatrix4x4 mPitchMatrix = CMatrix4x4::Identity;
    mPitchMatrix.mMat[1][1] = (float)cos(pitch);
    mPitchMatrix.mMat[1][2] = (float)sin(pitch);
    mPitchMatrix.mMat[2][1] = -(float)sin(pitch);
    mPitchMatrix.mMat[2][2] = (float)cos(pitch);

    CMatrix4x4 mYawMatrix = CMatrix4x4::Identity;
    mYawMatrix.mMat[0][0] = (float)cos(yaw);
    mYawMatrix.mMat[0][2] = -(float)sin(yaw);
    mYawMatrix.mMat[2][0] = (float)sin(yaw);
    mYawMatrix.mMat[2][2] = (float)cos(yaw);

    CMatrix4x4 mRollMatrix = CMatrix4x4::Identity;
    mRollMatrix.mMat[0][0] = (float)cos(roll);
    mRollMatrix.mMat[0][1] = (float)sin(roll);
    mRollMatrix.mMat[1][0] = -(float)sin(roll);
    mRollMatrix.mMat[1][1] = (float)cos(roll);

    return mPitchMatrix * mYawMatrix * mRollMatrix;
  }
  // ------------------------------------
  maths::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        mMatrix.mMat[i][j] = _mMatrix.mMat[j][i];
      }
    }
    return mMatrix;
  }
}