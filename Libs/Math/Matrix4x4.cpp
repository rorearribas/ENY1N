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
    m[0] = _m11; m[4] = _m12; m[8] = _m13; m[12] = _m14;
    m[1] = _m21; m[5] = _m22; m[9] = _m23; m[13] = _m24;
    m[2] = _m31; m[6] = _m32; m[10] = _m33; m[14] = _m34;
    m[3] = _m41; m[7] = _m42; m[11] = _m43; m[15] = _m44;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::operator*(const CMatrix4x4& _Other) const
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Zero;
    for (int iColumn = 0; iColumn < s_iColumnSize; ++iColumn)
    {
      for (int iRow = 0; iRow < s_iRowSize; ++iRow)
      {
        mMatrix.m[iColumn * 4 + iRow] =
          this->m[0 * 4 + iRow] * _Other.m[iColumn * 4 + 0] +
          this->m[1 * 4 + iRow] * _Other.m[iColumn * 4 + 1] +
          this->m[2 * 4 + iRow] * _Other.m[iColumn * 4 + 2] +
          this->m[3 * 4 + iRow] * _Other.m[iColumn * 4 + 3];
      }
    }
    return mMatrix;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreatePerspectiveMatrix(float _fFov, float _fAspectRatio, float _fNear, float _fFar)
  {
    // DirectX left-handed style
    float fFovRadians = math::Deg2Radians(_fFov);
    float fTan = tanf(fFovRadians / 2.0f);

    CMatrix4x4 mPerspective = CMatrix4x4::Identity;
    mPerspective.m[0] = 1.0f / (_fAspectRatio * fTan);
    mPerspective.m[5] = 1.0f / fTan;
    mPerspective.m[10] = _fFar / (_fFar - _fNear);
    mPerspective.m[11] = 1.0f;
    mPerspective.m[14] = -_fNear * _fFar / (_fFar - _fNear);
    mPerspective.m[15] = 0.0f;
    return mPerspective;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreateOrtographicMatrix(float _fWidth, float _fHeight, float _fNear, float _fFar)
  {
    // DirectX style
    CMatrix4x4 mOrtographic = CMatrix4x4::Identity;
    mOrtographic.m[0] = 2.0f / _fWidth;
    mOrtographic.m[5] = 2.0f / _fHeight;
    mOrtographic.m[10] = 1.0f / (_fNear - _fFar);
    mOrtographic.m[14] = -_fNear / (_fNear - _fFar);
    mOrtographic.m[15] = 1.0f;
    return mOrtographic;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _v3Pos, const CVector3& _v3Target, const CVector3& _v3Up)
  {
    // Get dir
    CVector3 v3Forward = math::CVector3::Normalize(_v3Target - _v3Pos);
    // Right
    CVector3 v3Right = math::CVector3::Normalize(_v3Up.Cross(v3Forward));
    // Up
    CVector3 v3Up = math::CVector3::Normalize(v3Forward.Cross(v3Right));

    // Compose matrix
    CMatrix4x4 mLookAt = CMatrix4x4::Identity;

    mLookAt.m[0] = v3Right.x;
    mLookAt.m[4] = v3Right.y;
    mLookAt.m[8] = v3Right.z;
    mLookAt.m[3] = 0.0f;

    mLookAt.m[1] = v3Up.x;
    mLookAt.m[5] = v3Up.y;
    mLookAt.m[9] = v3Up.z;
    mLookAt.m[7] = 0.0f;

    mLookAt.m[2] = v3Forward.x;
    mLookAt.m[6] = v3Forward.y;
    mLookAt.m[10] = v3Forward.z;
    mLookAt.m[11] = 0.0f;

    mLookAt.m[12] = -v3Right.Dot(_v3Pos);
    mLookAt.m[13] = -v3Up.Dot(_v3Pos);
    mLookAt.m[14] = -v3Forward.Dot(_v3Pos);
    mLookAt.m[15] = 1.0f;

    return mLookAt;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::RotationAxis(const CVector3& _v3Axis, float _fAngle)
  {
    // We are supposing that the axis is normalized!!
    float fCos = cosf(_fAngle);
    float fSin = sinf(_fAngle);
    float fOffset = 1.0f - fCos;

    CMatrix4x4 mRot(CMatrix4x4::Identity);
    mRot.m[0] = _v3Axis.x * _v3Axis.x * fOffset + fCos;
    mRot.m[4] = _v3Axis.x * _v3Axis.y * fOffset - _v3Axis.z * fSin;
    mRot.m[8] = _v3Axis.x * _v3Axis.z * fOffset + _v3Axis.y * fSin;

    mRot.m[1] = _v3Axis.y * _v3Axis.x * fOffset + _v3Axis.z * fSin;
    mRot.m[5] = _v3Axis.y * _v3Axis.y * fOffset + fCos;
    mRot.m[9] = _v3Axis.y * _v3Axis.z * fOffset - _v3Axis.x * fSin;

    mRot.m[2] = _v3Axis.z * _v3Axis.x * fOffset - _v3Axis.y * fSin;
    mRot.m[6] = _v3Axis.z * _v3Axis.y * fOffset + _v3Axis.x * fSin;
    mRot.m[10] = _v3Axis.z * _v3Axis.z * fOffset + fCos;

    mRot.m[15] = 1.0f;

    return mRot;
  }
  // ------------------------------------
  // https://iquilezles.org/articles/noacos/
  math::CMatrix4x4 CMatrix4x4::AlignMatrix(const CVector3& _v3Current, const CVector3& _v3Target)
  {
    const float fDot = math::CVector3::Dot(_v3Current, _v3Target);
    if (std::abs(fDot) > (1.0f - math::s_fEpsilon3))
    {
      if (fDot > 0.0f)
      {
        return Identity;
      }

      // Calculate 180 degrees
      math::CVector3 v3Orthogonal = math::CVector3::Cross(_v3Current, math::CVector3::Right);
      if (v3Orthogonal.Magnitude() < math::s_fEpsilon3)
      {
        v3Orthogonal = math::CVector3::Cross(_v3Current, math::CVector3::Up);
      }
      v3Orthogonal.Normalize();
      return CMatrix4x4::RotationAxis(v3Orthogonal, math::s_fPI);
    }

    const math::CVector3 v3Cross = math::CVector3::Cross(_v3Current, _v3Target);
    const float fK = 1.0f / (1.0f + fDot);

    return CMatrix4x4
    (
      v3Cross.x * v3Cross.x * fK + fDot,
      v3Cross.y * v3Cross.x * fK - v3Cross.z,
      v3Cross.z * v3Cross.x * fK + v3Cross.y,
      0.0f,

      v3Cross.x * v3Cross.y * fK + v3Cross.z,
      v3Cross.y * v3Cross.y * fK + fDot,
      v3Cross.z * v3Cross.y * fK - v3Cross.x,
      0.0f,

      v3Cross.x * v3Cross.z * fK - v3Cross.y,
      v3Cross.y * v3Cross.z * fK + v3Cross.x,
      v3Cross.z * v3Cross.z * fK + fDot,
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    );
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Invert(const math::CMatrix4x4& _m)
  {
    // Calculate cofactors
    const float* fMatrix = _m;
    float fInvert[16];
    {
      fInvert[0] = fMatrix[5] * fMatrix[10] * fMatrix[15] -
      fMatrix[5] * fMatrix[11] * fMatrix[14] -
      fMatrix[9] * fMatrix[6] * fMatrix[15] +
      fMatrix[9] * fMatrix[7] * fMatrix[14] +
      fMatrix[13] * fMatrix[6] * fMatrix[11] -
      fMatrix[13] * fMatrix[7] * fMatrix[10];

      fInvert[4] = -fMatrix[4] * fMatrix[10] * fMatrix[15] +
      fMatrix[4] * fMatrix[11] * fMatrix[14] +
      fMatrix[8] * fMatrix[6] * fMatrix[15] -
      fMatrix[8] * fMatrix[7] * fMatrix[14] -
      fMatrix[12] * fMatrix[6] * fMatrix[11] +
      fMatrix[12] * fMatrix[7] * fMatrix[10];

      fInvert[8] = fMatrix[4] * fMatrix[9] * fMatrix[15] -
      fMatrix[4] * fMatrix[11] * fMatrix[13] -
      fMatrix[8] * fMatrix[5] * fMatrix[15] +
      fMatrix[8] * fMatrix[7] * fMatrix[13] +
      fMatrix[12] * fMatrix[5] * fMatrix[11] -
      fMatrix[12] * fMatrix[7] * fMatrix[9];

      fInvert[12] = -fMatrix[4] * fMatrix[9] * fMatrix[14] +
      fMatrix[4] * fMatrix[10] * fMatrix[13] +
      fMatrix[8] * fMatrix[5] * fMatrix[14] -
      fMatrix[8] * fMatrix[6] * fMatrix[13] -
      fMatrix[12] * fMatrix[5] * fMatrix[10] +
      fMatrix[12] * fMatrix[6] * fMatrix[9];

      fInvert[1] = -fMatrix[1] * fMatrix[10] * fMatrix[15] +
      fMatrix[1] * fMatrix[11] * fMatrix[14] +
      fMatrix[9] * fMatrix[2] * fMatrix[15] -
      fMatrix[9] * fMatrix[3] * fMatrix[14] -
      fMatrix[13] * fMatrix[2] * fMatrix[11] +
      fMatrix[13] * fMatrix[3] * fMatrix[10];

      fInvert[5] = fMatrix[0] * fMatrix[10] * fMatrix[15] -
      fMatrix[0] * fMatrix[11] * fMatrix[14] -
      fMatrix[8] * fMatrix[2] * fMatrix[15] +
      fMatrix[8] * fMatrix[3] * fMatrix[14] +
      fMatrix[12] * fMatrix[2] * fMatrix[11] -
      fMatrix[12] * fMatrix[3] * fMatrix[10];

      fInvert[9] = -fMatrix[0] * fMatrix[9] * fMatrix[15] +
      fMatrix[0] * fMatrix[11] * fMatrix[13] +
      fMatrix[8] * fMatrix[1] * fMatrix[15] -
      fMatrix[8] * fMatrix[3] * fMatrix[13] -
      fMatrix[12] * fMatrix[1] * fMatrix[11] +
      fMatrix[12] * fMatrix[3] * fMatrix[9];

      fInvert[13] = fMatrix[0] * fMatrix[9] * fMatrix[14] -
      fMatrix[0] * fMatrix[10] * fMatrix[13] -
      fMatrix[8] * fMatrix[1] * fMatrix[14] +
      fMatrix[8] * fMatrix[2] * fMatrix[13] +
      fMatrix[12] * fMatrix[1] * fMatrix[10] -
      fMatrix[12] * fMatrix[2] * fMatrix[9];

      fInvert[2] = fMatrix[1] * fMatrix[6] * fMatrix[15] -
      fMatrix[1] * fMatrix[7] * fMatrix[14] -
      fMatrix[5] * fMatrix[2] * fMatrix[15] +
      fMatrix[5] * fMatrix[3] * fMatrix[14] +
      fMatrix[13] * fMatrix[2] * fMatrix[7] -
      fMatrix[13] * fMatrix[3] * fMatrix[6];

      fInvert[6] = -fMatrix[0] * fMatrix[6] * fMatrix[15] +
      fMatrix[0] * fMatrix[7] * fMatrix[14] +
      fMatrix[4] * fMatrix[2] * fMatrix[15] -
      fMatrix[4] * fMatrix[3] * fMatrix[14] -
      fMatrix[12] * fMatrix[2] * fMatrix[7] +
      fMatrix[12] * fMatrix[3] * fMatrix[6];

      fInvert[10] = fMatrix[0] * fMatrix[5] * fMatrix[15] -
      fMatrix[0] * fMatrix[7] * fMatrix[13] -
      fMatrix[4] * fMatrix[1] * fMatrix[15] +
      fMatrix[4] * fMatrix[3] * fMatrix[13] +
      fMatrix[12] * fMatrix[1] * fMatrix[7] -
      fMatrix[12] * fMatrix[3] * fMatrix[5];

      fInvert[14] = -fMatrix[0] * fMatrix[5] * fMatrix[14] +
      fMatrix[0] * fMatrix[6] * fMatrix[13] +
      fMatrix[4] * fMatrix[1] * fMatrix[14] -
      fMatrix[4] * fMatrix[2] * fMatrix[13] -
      fMatrix[12] * fMatrix[1] * fMatrix[6] +
      fMatrix[12] * fMatrix[2] * fMatrix[5];

      fInvert[3] = -fMatrix[1] * fMatrix[6] * fMatrix[11] +
      fMatrix[1] * fMatrix[7] * fMatrix[10] +
      fMatrix[5] * fMatrix[2] * fMatrix[11] -
      fMatrix[5] * fMatrix[3] * fMatrix[10] -
      fMatrix[9] * fMatrix[2] * fMatrix[7] +
      fMatrix[9] * fMatrix[3] * fMatrix[6];

      fInvert[7] = fMatrix[0] * fMatrix[6] * fMatrix[11] -
      fMatrix[0] * fMatrix[7] * fMatrix[10] -
      fMatrix[4] * fMatrix[2] * fMatrix[11] +
      fMatrix[4] * fMatrix[3] * fMatrix[10] +
      fMatrix[8] * fMatrix[2] * fMatrix[7] -
      fMatrix[8] * fMatrix[3] * fMatrix[6];

      fInvert[11] = -fMatrix[0] * fMatrix[5] * fMatrix[11] +
      fMatrix[0] * fMatrix[7] * fMatrix[9] +
      fMatrix[4] * fMatrix[1] * fMatrix[11] -
      fMatrix[4] * fMatrix[3] * fMatrix[9] -
      fMatrix[8] * fMatrix[1] * fMatrix[7] +
      fMatrix[8] * fMatrix[3] * fMatrix[5];

      fInvert[15] = fMatrix[0] * fMatrix[5] * fMatrix[10] -
      fMatrix[0] * fMatrix[6] * fMatrix[9] -
      fMatrix[4] * fMatrix[1] * fMatrix[10] +
      fMatrix[4] * fMatrix[2] * fMatrix[9] +
      fMatrix[8] * fMatrix[1] * fMatrix[6] -
      fMatrix[8] * fMatrix[2] * fMatrix[5];
    }

    // Calculate determinant
    float fDet = fMatrix[0] * fInvert[0] + fMatrix[1] * fInvert[4] + fMatrix[2] * fInvert[8] + fMatrix[3] * fInvert[12];
    if (fDet < s_fEpsilon3)
    {
      return CMatrix4x4::Identity;
    }
    fDet = (1.0f / fDet);

    CMatrix4x4 mInvert = CMatrix4x4::Identity;
    for (uint32_t uI = 0; uI < (s_iColumnSize * s_iRowSize); uI++)
    {
      mInvert[uI] = fInvert[uI] * fDet;
    }

    return mInvert;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mTranspose = CMatrix4x4::Identity;
    for (int iRow = 0; iRow < s_iRowSize; ++iRow)
    {
      for (int iColumn = 0; iColumn < s_iColumnSize; ++iColumn)
      {
        mTranspose.m[iColumn * 4 + iRow] = _mMatrix.m[iRow * 4 + iColumn];
      }
    }
    return mTranspose;
  }
  // ------------------------------------
  void CMatrix4x4::Transpose()
  {
    *this = Transpose(*this);
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Translate(const CVector3& _v3Translate)
  {
    // Create translate matrix
    CMatrix4x4 mTranslate = CMatrix4x4::Identity;
    mTranslate.m[12] = _v3Translate.x;
    mTranslate.m[13] = _v3Translate.y;
    mTranslate.m[14] = _v3Translate.z;
    return mTranslate;
  }
  // ------------------------------------
  void CMatrix4x4::SetTranslate(const CVector3& _v3Translate)
  {
    // Set translate
    this->m[12] = _v3Translate.x;
    this->m[13] = _v3Translate.y;
    this->m[14] = _v3Translate.z;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetTranslate() const
  {
    return math::CVector3(m[12], m[13], m[14]);
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Scale(const CVector3& _v3Scale)
  {
    CMatrix4x4 mScale = CMatrix4x4::Identity;
    mScale.m[0] = _v3Scale.x;
    mScale.m[5] = _v3Scale.y;
    mScale.m[10] = _v3Scale.z;
    return mScale;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetScale() const
  {
    math::CVector3 v3Scale;
    v3Scale.x = sqrtf(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
    v3Scale.y = sqrtf(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
    v3Scale.z = sqrtf(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
    return v3Scale;
  }
  // ------------------------------------
  const bool CMatrix4x4::HasScaleUniform() const
  {
    math::CVector3 v3Scale = GetScale();
    return fabs(v3Scale.x - v3Scale.y) < math::s_fEpsilon5 && fabs(v3Scale.x - v3Scale.z) < math::s_fEpsilon5;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Rotation(const CVector3& _v3Rot)
  {
    float fPitch = math::Deg2Radians(_v3Rot.x);
    float fYaw = math::Deg2Radians(_v3Rot.y);
    float fRoll = math::Deg2Radians(_v3Rot.z);

    // Pitch (X)
    CMatrix4x4 mPitch = CMatrix4x4::Identity;
    mPitch.m[5] = cos(fPitch);
    mPitch.m[6] = sin(fPitch);
    mPitch.m[9] = -sin(fPitch);
    mPitch.m[10] = cos(fPitch);

    // Roll (Y)
    CMatrix4x4 mRoll = CMatrix4x4::Identity;
    mRoll.m[0] = cos(fRoll);
    mRoll.m[1] = sin(fRoll);
    mRoll.m[4] = -sin(fRoll);
    mRoll.m[5] = cos(fRoll);

    // Yaw (Z)
    CMatrix4x4 mYaw = CMatrix4x4::Identity;
    mYaw.m[0] = cos(fYaw);
    mYaw.m[2] = -sin(fYaw);
    mYaw.m[8] = sin(fYaw);
    mYaw.m[10] = cos(fYaw);

    return mYaw * mPitch * mRoll;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetRotation() const
  {
    math::CVector3 v3Rot = math::CVector3::Zero;
    // Get axis with uniform scale
    math::CVector3 xAxis = math::CVector3(m[0], m[1], m[2]);
    math::CVector3 yAxis = math::CVector3(m[4], m[5], m[6]);
    math::CVector3 zAxis = math::CVector3(m[8], m[9], m[10]);
    // Normalize
    xAxis.Normalize();
    yAxis.Normalize();
    zAxis.Normalize();
    // Calculate rot
    v3Rot.x = math::Rad2Degrees(atan2f(-zAxis.y, zAxis.z)); // pitch
    v3Rot.y = math::Rad2Degrees(atan2f(zAxis.x, sqrtf(zAxis.y * zAxis.y + zAxis.z * zAxis.z))); // roll
    v3Rot.z = math::Rad2Degrees(atan2f(-yAxis.x, xAxis.x)); // yaw
    return v3Rot;
  }
}