#pragma once
#include <array>

namespace maths
{
  class CMatrix4x4
  {
    static constexpr int s_iMatrixSize = 4;

  public:
    typedef float TMatrix4x4[s_iMatrixSize][s_iMatrixSize];
    static const CMatrix4x4 Identity;
    static const CMatrix4x4 Zero;

    CMatrix4x4(const float _mMatrix[s_iMatrixSize][s_iMatrixSize]);
    CMatrix4x4
    (
      float _m11, float _m12, float _m13, float _m14, 
      float _m21, float _m22, float _m23, float _m24, 
      float _m31, float _m32, float _m33, float _m34, 
      float _m41, float _m42, float _m43, float _m44
    );

  private:
    TMatrix4x4 m_mMatrix;
  };
}



