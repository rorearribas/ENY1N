#include "Matrix4x4.h"

namespace maths
{
  const CMatrix4x4 CMatrix4x4::mIdentity = 
  {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };
  const CMatrix4x4 CMatrix4x4::mZero =
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
        m_mMatrix[iRowIdx][iColumnIdx] = _mMatrix[iRowIdx][iColumnIdx];
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
    m_mMatrix[0][0] = _m11; m_mMatrix[0][1] = _m12; m_mMatrix[0][2] = _m13; m_mMatrix[0][3] = _m14;
    m_mMatrix[1][0] = _m21; m_mMatrix[1][1] = _m22; m_mMatrix[1][2] = _m23; m_mMatrix[1][3] = _m24;
    m_mMatrix[2][0] = _m31; m_mMatrix[2][1] = _m32; m_mMatrix[2][2] = _m33; m_mMatrix[2][3] = _m34;
    m_mMatrix[3][0] = _m41; m_mMatrix[3][1] = _m42; m_mMatrix[3][2] = _m43; m_mMatrix[3][3] = _m44;
  }
}