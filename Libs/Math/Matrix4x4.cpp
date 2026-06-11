#include "Matrix4x4.h"
#include "Math.h"

#include <xmmintrin.h>
#include <immintrin.h>

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
    CMatrix4x4 mMatrix = CMatrix4x4::Identity;

    __m128 r0 = _mm_load_ps(&this->m[0]);
    __m128 r1 = _mm_load_ps(&this->m[4]);
    __m128 r2 = _mm_load_ps(&this->m[8]);
    __m128 r3 = _mm_load_ps(&this->m[12]);

    __m256 row0 = _mm256_setr_m128(r0, r0); // [r0.x, r0.y, r0.z, r0.w, r0.x, r0.y, r0.z, r0.w]
    __m256 row1 = _mm256_setr_m128(r1, r1);
    __m256 row2 = _mm256_setr_m128(r2, r2);
    __m256 row3 = _mm256_setr_m128(r3, r3);

    // --- First 2 columns (Elements 0 to 7) ---
    {
      __m256 e0 = _mm256_setr_ps(_Other.m[0], _Other.m[0], _Other.m[0], _Other.m[0], _Other.m[4], _Other.m[4], _Other.m[4], _Other.m[4]);
      __m256 e1 = _mm256_setr_ps(_Other.m[1], _Other.m[1], _Other.m[1], _Other.m[1], _Other.m[5], _Other.m[5], _Other.m[5], _Other.m[5]);
      __m256 e2 = _mm256_setr_ps(_Other.m[2], _Other.m[2], _Other.m[2], _Other.m[2], _Other.m[6], _Other.m[6], _Other.m[6], _Other.m[6]);
      __m256 e3 = _mm256_setr_ps(_Other.m[3], _Other.m[3], _Other.m[3], _Other.m[3], _Other.m[7], _Other.m[7], _Other.m[7], _Other.m[7]);

      __m256 dst = _mm256_mul_ps(e0, row0);
      dst = _mm256_fmadd_ps(e1, row1, dst);
      dst = _mm256_fmadd_ps(e2, row2, dst);
      dst = _mm256_fmadd_ps(e3, row3, dst);

      _mm256_store_ps(&mMatrix.m[0], dst);
    }

    // --- Last 2 columns (Elements 8 to 15) ---
    {
      __m256 e0 = _mm256_setr_ps(_Other.m[8], _Other.m[8], _Other.m[8], _Other.m[8], _Other.m[12], _Other.m[12], _Other.m[12], _Other.m[12]);
      __m256 e1 = _mm256_setr_ps(_Other.m[9], _Other.m[9], _Other.m[9], _Other.m[9], _Other.m[13], _Other.m[13], _Other.m[13], _Other.m[13]);
      __m256 e2 = _mm256_setr_ps(_Other.m[10], _Other.m[10], _Other.m[10], _Other.m[10], _Other.m[14], _Other.m[14], _Other.m[14], _Other.m[14]);
      __m256 e3 = _mm256_setr_ps(_Other.m[11], _Other.m[11], _Other.m[11], _Other.m[11], _Other.m[15], _Other.m[15], _Other.m[15], _Other.m[15]);

      __m256 dst = _mm256_mul_ps(e0, row0);
      dst = _mm256_fmadd_ps(e1, row1, dst);
      dst = _mm256_fmadd_ps(e2, row2, dst);
      dst = _mm256_fmadd_ps(e3, row3, dst);

      _mm256_store_ps(&mMatrix.m[8], dst);
    }

    return mMatrix;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::operator*(const math::CVector3& _v3Other) const
  {
    // Load the matching components of the matrix (Rows/Columns depending on layout)
    __m128 r0 = _mm_load_ps(&m[0]);  // [m[0],  m[1],  m[2],  m[3]]
    __m128 r1 = _mm_load_ps(&m[4]);  // [m[4],  m[5],  m[6],  m[7]]
    __m128 r2 = _mm_load_ps(&m[8]);  // [m[8],  m[9],  m[10], m[11]]
    __m128 r3 = _mm_load_ps(&m[12]);// [m[12], m[13], m[14], m[15]]

    // Broadcast each component of the vector into all 4 channels of an SSE register
    __m128 vX = _mm_set1_ps(_v3Other.x);
    __m128 vY = _mm_set1_ps(_v3Other.y);
    __m128 vZ = _mm_set1_ps(_v3Other.z);

    // Perform the linear combination using Fused Multiply-Add (FMA)
    // result = vX * m0 + vY * m4 + vZ * m8 + 1.0 * m12
    __m128 vResult = _mm_mul_ps(vX, r0);
    vResult = _mm_fmadd_ps(vY, r1, vResult);
    vResult = _mm_fmadd_ps(vZ, r2, vResult);
    vResult = _mm_add_ps(vResult, r3);

    // Extract the final x, y, z components from the register back to CVector3
    float alignas(16) fRes[4];
    _mm_store_ps(fRes, vResult);

    return math::CVector3(fRes[0], fRes[1], fRes[2]);
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
    // DirectX left-handed style
    CMatrix4x4 mOrtographic = CMatrix4x4::Identity;
    mOrtographic.m[0] = 2.0f / _fWidth;
    mOrtographic.m[5] = 2.0f / _fHeight;
    mOrtographic.m[10] = 1.0f / (_fFar - _fNear);
    mOrtographic.m[14] = -_fNear / (_fFar - _fNear);
    mOrtographic.m[15] = 1.0f;
    return mOrtographic;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetAxisX() const
  {
    return math::CVector3::Normalize(math::CVector3(m[0], m[1], m[2]));;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetAxisY() const
  {
    return math::CVector3::Normalize(math::CVector3(m[4], m[5], m[6]));
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetAxisZ() const
  {
    return math::CVector3::Normalize(math::CVector3(m[8], m[9], m[10]));
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::LookAt(const CVector3& _v3Pos, const CVector3& _v3Target, const CVector3& _v3Up)
  {
    // Get dir
    CVector3 v3Dir = math::CVector3::Normalize(_v3Target - _v3Pos);

    // Right
    CVector3 v3Right = math::CVector3::Normalize(_v3Up.Cross(v3Dir));
    if (v3Right.Magnitude() <= math::s_fEpsilon7)
    {
      // Set alternative direction
      v3Right = _v3Up.Cross(math::CVector3::Forward); 
    }

    // Up
    CVector3 v3Up = math::CVector3::Normalize(v3Dir.Cross(v3Right));

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

    mLookAt.m[2] = v3Dir.x;
    mLookAt.m[6] = v3Dir.y;
    mLookAt.m[10] = v3Dir.z;
    mLookAt.m[11] = 0.0f;

    mLookAt.m[12] = -v3Right.Dot(_v3Pos);
    mLookAt.m[13] = -v3Up.Dot(_v3Pos);
    mLookAt.m[14] = -v3Dir.Dot(_v3Pos);
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
    // 1. Calculate cofactors
    float fInvert[16];
    {
      fInvert[0] = _m[5] * _m[10] * _m[15] - _m[5] * _m[11] * _m[14] - _m[9] * _m[6] * _m[15] + _m[9] * _m[7] * _m[14] + _m[13] * _m[6] * _m[11] - _m[13] * _m[7] * _m[10];
      fInvert[4] = -_m[4] * _m[10] * _m[15] + _m[4] * _m[11] * _m[14] + _m[8] * _m[6] * _m[15] - _m[8] * _m[7] * _m[14] - _m[12] * _m[6] * _m[11] + _m[12] * _m[7] * _m[10];
      fInvert[8] = _m[4] * _m[9] * _m[15] - _m[4] * _m[11] * _m[13] - _m[8] * _m[5] * _m[15] + _m[8] * _m[7] * _m[13] + _m[12] * _m[5] * _m[11] - _m[12] * _m[7] * _m[9];
      fInvert[12] = -_m[4] * _m[9] * _m[14] + _m[4] * _m[10] * _m[13] + _m[8] * _m[5] * _m[14] - _m[8] * _m[6] * _m[13] - _m[12] * _m[5] * _m[10] + _m[12] * _m[6] * _m[9];
      fInvert[1] = -_m[1] * _m[10] * _m[15] + _m[1] * _m[11] * _m[14] + _m[9] * _m[2] * _m[15] - _m[9] * _m[3] * _m[14] - _m[13] * _m[2] * _m[11] + _m[13] * _m[3] * _m[10];
      fInvert[5] = _m[0] * _m[10] * _m[15] - _m[0] * _m[11] * _m[14] - _m[8] * _m[2] * _m[15] + _m[8] * _m[3] * _m[14] + _m[12] * _m[2] * _m[11] - _m[12] * _m[3] * _m[10];
      fInvert[9] = -_m[0] * _m[9] * _m[15] + _m[0] * _m[11] * _m[13] + _m[8] * _m[1] * _m[15] - _m[8] * _m[3] * _m[13] - _m[12] * _m[1] * _m[11] + _m[12] * _m[3] * _m[9];
      fInvert[13] = _m[0] * _m[9] * _m[14] - _m[0] * _m[10] * _m[13] - _m[8] * _m[1] * _m[14] + _m[8] * _m[2] * _m[13] + _m[12] * _m[1] * _m[10] - _m[12] * _m[2] * _m[9];
      fInvert[2] = _m[1] * _m[6] * _m[15] - _m[1] * _m[7] * _m[14] - _m[5] * _m[2] * _m[15] + _m[5] * _m[3] * _m[14] + _m[13] * _m[2] * _m[7] - _m[13] * _m[3] * _m[6];
      fInvert[6] = -_m[0] * _m[6] * _m[15] + _m[0] * _m[7] * _m[14] + _m[4] * _m[2] * _m[15] - _m[4] * _m[3] * _m[14] - _m[12] * _m[2] * _m[7] + _m[12] * _m[3] * _m[6];
      fInvert[10] = _m[0] * _m[5] * _m[15] - _m[0] * _m[7] * _m[13] - _m[4] * _m[1] * _m[15] + _m[4] * _m[3] * _m[13] + _m[12] * _m[1] * _m[7] - _m[12] * _m[3] * _m[5];
      fInvert[14] = -_m[0] * _m[5] * _m[14] + _m[0] * _m[6] * _m[13] + _m[4] * _m[1] * _m[14] - _m[4] * _m[2] * _m[13] - _m[12] * _m[1] * _m[6] + _m[12] * _m[2] * _m[5];
      fInvert[3] = -_m[1] * _m[6] * _m[11] + _m[1] * _m[7] * _m[10] + _m[5] * _m[2] * _m[11] - _m[5] * _m[3] * _m[10] - _m[9] * _m[2] * _m[7] + _m[9] * _m[3] * _m[6];
      fInvert[7] = _m[0] * _m[6] * _m[11] - _m[0] * _m[7] * _m[10] - _m[4] * _m[2] * _m[11] + _m[4] * _m[3] * _m[10] + _m[8] * _m[2] * _m[7] - _m[8] * _m[3] * _m[6];
      fInvert[11] = -_m[0] * _m[5] * _m[11] + _m[0] * _m[7] * _m[9] + _m[4] * _m[1] * _m[11] - _m[4] * _m[3] * _m[9] - _m[8] * _m[1] * _m[7] + _m[8] * _m[3] * _m[5];
      fInvert[15] = _m[0] * _m[5] * _m[10] - _m[0] * _m[6] * _m[9] - _m[4] * _m[1] * _m[10] + _m[4] * _m[2] * _m[9] + _m[8] * _m[1] * _m[6] - _m[8] * _m[2] * _m[5];
    }

    // 2. Calculate determinant
    float fDet = _m[0] * fInvert[0] + _m[1] * fInvert[4] + _m[2] * fInvert[8] + _m[3] * fInvert[12];
    if (fabsf(fDet) < math::s_fEpsilon7)
    {
      return CMatrix4x4::Identity;
    }

    // 3. SSE Optimization: Multiply all 4 columns in parallel, replacing the scalar loop
    __m128 vDetInv = _mm_set1_ps(1.0f / fDet);

    math::CMatrix4x4 mInvert;
    _mm_store_ps(&mInvert.m[0], _mm_mul_ps(_mm_loadu_ps(&fInvert[0]), vDetInv));
    _mm_store_ps(&mInvert.m[4], _mm_mul_ps(_mm_loadu_ps(&fInvert[4]), vDetInv));
    _mm_store_ps(&mInvert.m[8], _mm_mul_ps(_mm_loadu_ps(&fInvert[8]), vDetInv));
    _mm_store_ps(&mInvert.m[12], _mm_mul_ps(_mm_loadu_ps(&fInvert[12]), vDetInv));

    return mInvert;
  }
  // ------------------------------------
  void CMatrix4x4::Invert()
  {
    *this = Invert(*this);
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::Transpose(const CMatrix4x4& _mMatrix)
  {
    CMatrix4x4 mResult = CMatrix4x4::Identity;

    // Load the 4 rows of the source matrix into 128-bit registers
    __m128 r0 = _mm_load_ps(&_mMatrix.m[0]);  // Row 0: [m0,  m1,  m2,  m3]
    __m128 r1 = _mm_load_ps(&_mMatrix.m[4]);  // Row 1: [m4,  m5,  m6,  m7]
    __m128 r2 = _mm_load_ps(&_mMatrix.m[8]);  // Row 2: [m8,  m9,  m10, m11]
    __m128 r3 = _mm_load_ps(&_mMatrix.m[12]); // Row 3: [m12, m13, m14, m15]

    // Interleave lower and upper parts of the rows
    __m128 tmp0 = _mm_unpacklo_ps(r0, r1); // [m0, m4, m1, m5]
    __m128 tmp1 = _mm_unpacklo_ps(r2, r3); // [m8, m12, m9, m13]
    __m128 tmp2 = _mm_unpackhi_ps(r0, r1); // [m2, m6, m3, m7]
    __m128 tmp3 = _mm_unpackhi_ps(r2, r3); // [m10, m14, m11, m15]

    // Combine temporary registers into the final transposed rows
    __m128 out0 = _mm_movelh_ps(tmp0, tmp1); // Row 0 result: [m0, m4, m8, m12]
    __m128 out1 = _mm_movehl_ps(tmp1, tmp0); // Row 1 result: [m1, m5, m9, m13]
    __m128 out2 = _mm_movelh_ps(tmp2, tmp3); // Row 2 result: [m2, m6, m10, m14]
    __m128 out3 = _mm_movehl_ps(tmp3, tmp2); // Row 3 result: [m3, m7, m11, m15]

    // Store the transposed rows directly into the result matrix
    _mm_store_ps(&mResult.m[0], out0);
    _mm_store_ps(&mResult.m[4], out1);
    _mm_store_ps(&mResult.m[8], out2);
    _mm_store_ps(&mResult.m[12], out3);

    return mResult;
  }
  // ------------------------------------
  void CMatrix4x4::Transpose()
  {
    *this = Transpose(*this);
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreateTranslation(const CVector3& _v3Translate)
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
  math::CMatrix4x4 CMatrix4x4::CreateScale(const CVector3& _v3Scale)
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
    math::CVector3 v3Scale = math::CVector3::Zero;
    v3Scale.x = sqrtf(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
    v3Scale.y = sqrtf(m[4] * m[4] + m[5] * m[5] + m[6] * m[6]);
    v3Scale.z = sqrtf(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
    return v3Scale;
  }
  // ------------------------------------
  math::CMatrix4x4 CMatrix4x4::CreateRotation(const CVector3& _v3Rot)
  {
    float fPitch = math::Deg2Radians(_v3Rot.x); // X
    float fYaw = math::Deg2Radians(_v3Rot.y);   // Y
    float fRoll = math::Deg2Radians(_v3Rot.z);  // Z

    // Pitch (X)
    CMatrix4x4 mPitch = CMatrix4x4::Identity;
    mPitch.m[5] = cosf(fPitch);
    mPitch.m[6] = sinf(fPitch);
    mPitch.m[9] = -sinf(fPitch);
    mPitch.m[10] = cosf(fPitch);

    // Yaw (Y)
    CMatrix4x4 mYaw = CMatrix4x4::Identity;
    mYaw.m[0] = cosf(fYaw);
    mYaw.m[2] = -sinf(fYaw);
    mYaw.m[8] = sinf(fYaw);
    mYaw.m[10] = cosf(fYaw);

    // Roll (Z)
    CMatrix4x4 mRoll = CMatrix4x4::Identity;
    mRoll.m[0] = cosf(fRoll);
    mRoll.m[1] = sinf(fRoll);
    mRoll.m[4] = -sinf(fRoll);
    mRoll.m[5] = cosf(fRoll);

    return mYaw * mPitch * mRoll;
  }
  // ------------------------------------
  math::CVector3 CMatrix4x4::GetRotation() const
  {
    math::CVector3 v3Rotation = math::CVector3::Zero;

    // Get axis
    math::CVector3 xAxis = math::CVector3::Normalize(math::CVector3(m[0], m[1], m[2]));
    math::CVector3 yAxis = math::CVector3::Normalize(math::CVector3(m[4], m[5], m[6]));
    math::CVector3 zAxis = math::CVector3::Normalize(math::CVector3(m[8], m[9], m[10]));

    // Get pitch
    float fPitch = asinf(-zAxis.y);
    if (cosf(fPitch) > math::s_fEpsilon5)
    {
      v3Rotation.x = math::Rad2Degrees(fPitch);
      v3Rotation.y = math::Rad2Degrees(atan2f(zAxis.x, zAxis.z));
      v3Rotation.z = math::Rad2Degrees(atan2f(xAxis.y, yAxis.y));
    }
    else // Gimbal Lock
    {
      v3Rotation.x = math::Rad2Degrees(fPitch);
      v3Rotation.y = math::Rad2Degrees(atan2f(-yAxis.x, xAxis.x));
      v3Rotation.z = 0.0f;
    }

    return v3Rotation;
  }
}