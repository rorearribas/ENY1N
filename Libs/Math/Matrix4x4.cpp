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
    m16[0] = _m11; m16[4] = _m12; m16[8]  = _m13; m16[12] = _m14;
    m16[1] = _m21; m16[5] = _m22; m16[9]  = _m23; m16[13] = _m24;
    m16[2] = _m31; m16[6] = _m32; m16[10] = _m33; m16[14] = _m34;
    m16[3] = _m41; m16[7] = _m42; m16[11] = _m43; m16[15] = _m44;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::operator*(const CMatrix4x4& _Other) const
  {
    CMatrix4x4 mMatrix = CMatrix4x4::Zero;
    for (int iColumn = 0; iColumn < s_iColumnSize; ++iColumn)
    {
      for (int iRow = 0; iRow < s_iRowSize; ++iRow)
      {
        mMatrix.m16[iColumn * 4 + iRow] =
          this->m16[0 * 4 + iRow] * _Other.m16[iColumn * 4 + 0] +
          this->m16[1 * 4 + iRow] * _Other.m16[iColumn * 4 + 1] +
          this->m16[2 * 4 + iRow] * _Other.m16[iColumn * 4 + 2] +
          this->m16[3 * 4 + iRow] * _Other.m16[iColumn * 4 + 3];
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
    mPerspective.m16[0] = 1.0f / (_fAspectRatio * fTan);
    mPerspective.m16[5] = 1.0f / fTan;
    mPerspective.m16[10] = _fFar / (_fFar - _fNear);
    mPerspective.m16[11] = 1.0f;
    mPerspective.m16[14] = -_fNear * _fFar / (_fFar - _fNear);
    mPerspective.m16[15] = 0.0f;
    return mPerspective;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreateOrtographicMatrix(float _fWidth, float _fHeight, float _fNear, float _fFar)
  {
    // DirectX style
    CMatrix4x4 mOrtographic = CMatrix4x4::Identity;
    mOrtographic.m16[0] = 2.0f / _fWidth;
    mOrtographic.m16[5] = 2.0f / _fHeight;
    mOrtographic.m16[10] = 1.0f / (_fNear - _fFar);
    mOrtographic.m16[14] = -_fNear / (_fNear - _fFar);
    mOrtographic.m16[15] = 1.0f;
    return mOrtographic;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _v3Pos, const CVector3& _v3Target, const CVector3& _v3Up)
  {
    // Get dir
    CVector3 v3Forward = (_v3Target - _v3Pos).Normalize();
    // Right
    CVector3 v3Right = _v3Up.Cross(v3Forward).Normalize();
    // Up
    CVector3 v3Up = v3Forward.Cross(v3Right).Normalize();

    // Compose matrix
    CMatrix4x4 mLookAt = CMatrix4x4::Identity;

    mLookAt.m16[0] = v3Right.X;
    mLookAt.m16[4] = v3Right.Y;
    mLookAt.m16[8] = v3Right.Z;
    mLookAt.m16[3] = 0.0f;

    mLookAt.m16[1] = v3Up.X;
    mLookAt.m16[5] = v3Up.Y;
    mLookAt.m16[9] = v3Up.Z;
    mLookAt.m16[7] = 0.0f;

    mLookAt.m16[2] = v3Forward.X;
    mLookAt.m16[6] = v3Forward.Y;
    mLookAt.m16[10] = v3Forward.Z;
    mLookAt.m16[11] = 0.0f;

    mLookAt.m16[12] = -v3Right.Dot(_v3Pos);
    mLookAt.m16[13] = -v3Up.Dot(_v3Pos);
    mLookAt.m16[14] = -v3Forward.Dot(_v3Pos);
    mLookAt.m16[15] = 1.0f;

    return mLookAt;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Translate(const CVector3& _v3Translate)
  {
    CMatrix4x4 mTranslate = CMatrix4x4::Identity;
    mTranslate.m16[12] = _v3Translate.X;
    mTranslate.m16[13] = _v3Translate.Y;
    mTranslate.m16[14] = _v3Translate.Z;
    return mTranslate;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Scale(const CVector3& _v3Scale)
  {
    CMatrix4x4 mScale = CMatrix4x4::Identity;
    mScale.m16[0] = _v3Scale.X;
    mScale.m16[5] = _v3Scale.Y;
    mScale.m16[10] = _v3Scale.Z;
    return mScale;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Rotation(const CVector3& _v3Rot)
  {
    float fPitch = math::Deg2Radians(_v3Rot.X);
    float fYaw = math::Deg2Radians(_v3Rot.Y);
    float fRoll = math::Deg2Radians(_v3Rot.Z);

    // Roll (Z)
    CMatrix4x4 mRoll = CMatrix4x4::Identity;
    mRoll.m16[0] = cos(fRoll);
    mRoll.m16[1] = sin(fRoll);
    mRoll.m16[4] = -sin(fRoll);
    mRoll.m16[5] = cos(fRoll);

    // Pitch (X)
    CMatrix4x4 mPitch = CMatrix4x4::Identity;
    mPitch.m16[5] = cos(fPitch);
    mPitch.m16[6] = sin(fPitch);
    mPitch.m16[9] = -sin(fPitch);
    mPitch.m16[10] = cos(fPitch);

    // Yaw (Y)
    CMatrix4x4 mYaw = CMatrix4x4::Identity;
    mYaw.m16[0] = cos(fYaw);
    mYaw.m16[2] = -sin(fYaw);
    mYaw.m16[8] = sin(fYaw);
    mYaw.m16[10] = cos(fYaw);

    return mYaw * mPitch * mRoll;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::RotationAxis(const CVector3& _v3Axis, float _fAngle)
  {
    // We are supposing that the axis is normalized!!
    float fCos = cosf(_fAngle);
    float fSin = sinf(_fAngle);
    float fOffset = 1.0f - fCos;

    CMatrix4x4 mRot(CMatrix4x4::Identity);
    mRot.m16[0] = _v3Axis.X * _v3Axis.X * fOffset + fCos;
    mRot.m16[4] = _v3Axis.X * _v3Axis.Y * fOffset - _v3Axis.Z * fSin;
    mRot.m16[8] = _v3Axis.X * _v3Axis.Z * fOffset + _v3Axis.Y * fSin;

    mRot.m16[1] = _v3Axis.Y * _v3Axis.X * fOffset + _v3Axis.Z * fSin;
    mRot.m16[5] = _v3Axis.Y * _v3Axis.Y * fOffset + fCos;
    mRot.m16[9] = _v3Axis.Y * _v3Axis.Z * fOffset - _v3Axis.X * fSin;

    mRot.m16[2] = _v3Axis.Z * _v3Axis.X * fOffset - _v3Axis.Y * fSin;
    mRot.m16[6] = _v3Axis.Z * _v3Axis.Y * fOffset + _v3Axis.X * fSin;
    mRot.m16[10] = _v3Axis.Z * _v3Axis.Z * fOffset + fCos;

    mRot.m16[15] = 1.0f;

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
      v3Cross.X * v3Cross.X * fK + fDot,
      v3Cross.Y * v3Cross.X * fK - v3Cross.Z,
      v3Cross.Z * v3Cross.X * fK + v3Cross.Y,
      0.0f,

      v3Cross.X * v3Cross.Y * fK + v3Cross.Z,
      v3Cross.Y * v3Cross.Y * fK + fDot, 
      v3Cross.Z * v3Cross.Y * fK - v3Cross.X,
      0.0f,

      v3Cross.X * v3Cross.Z * fK - v3Cross.Y,
      v3Cross.Y * v3Cross.Z * fK + v3Cross.X,
      v3Cross.Z * v3Cross.Z * fK + fDot,
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    );
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mTranspose = CMatrix4x4::Identity;
    for (int iRow = 0; iRow < s_iRowSize; ++iRow)
    {
      for (int iColumn = 0; iColumn < s_iColumnSize; ++iColumn)
      {
        mTranspose.m16[iColumn * 4 + iRow] = _mMatrix.m16[iRow * 4 + iColumn];
      }
    }
    return mTranspose;
  }
}