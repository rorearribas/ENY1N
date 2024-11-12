#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer.h"

namespace render
{
  class CCamera
  {
  public:
    struct SCameraSettings 
    {
      float m_fFov = 45.0f;
      float m_fAspectRatio = 1.7777f;
      float m_fNear = 0.01f;
      float m_fFar = 100000.0f;
    };

    CCamera();
    ~CCamera() {}

    void SetupCamera(const SCameraSettings& _oCameraSettings);
    void Update();

    void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
    const maths::CMatrix4x4& GetViewMatrix() const;
    const maths::CMatrix4x4& GetProjectionMatrix() const;

    const maths::CVector3& GetPositionVector() const;
    const maths::CVector3& GetPositionFloat3() const;
    const maths::CVector3& GetRotationVector() const;
    const maths::CVector3& GetRotationFloat3() const;

    void SetPosition(const maths::CVector3& pos);
    void SetPosition(float x, float y, float z);
    void AdjustPosition(const maths::CVector3& pos);
    void AdjustPosition(float x, float y, float z);
    void SetRotation(const maths::CVector3& rot);
    void SetRotation(float x, float y, float z);
    void AdjustRotation(const maths::CVector3& rot);
    void AdjustRotation(float x, float y, float z);
    void SetLookAtPos(const maths::CVector3& _v3LookAtPos);

    const maths::CVector3& GetForwardVector();
    const maths::CVector3& GetRightVector();
    const maths::CVector3& GetBackwardVector();
    const maths::CVector3& GetLeftVector();

  private:
    void UpdateViewMatrix();

    ConstantBuffer<SConstantBuffer> m_oConstantBuffer;
    maths::CMatrix4x4 m_mViewMatrix = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 m_mProjectionMatrix = maths::CMatrix4x4::Identity;

    maths::CVector3 posVector = maths::CVector3::Zero;
    maths::CVector3 rotVector = maths::CVector3::Zero;

    maths::CVector3 m_vPos = maths::CVector3::Zero;
    maths::CVector3 m_vRot = maths::CVector3::Zero;

    maths::CVector3 vec_forward;
    maths::CVector3 vec_left;
    maths::CVector3 vec_right;
    maths::CVector3 vec_backward;
  };
}

