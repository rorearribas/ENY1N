#pragma once
#include "Libs/Maths/Vector3.h"
#include "Libs/Maths/Matrix4x4.h"
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"

namespace render
{
  class CCamera
  {
  public:
    CCamera();
    ~CCamera() {}

    void Update(float _fDeltaTime);

    const maths::CMatrix4x4& GetViewMatrix() const { return m_mViewMatrix; }
    const maths::CMatrix4x4& GetProjectionMatrix() const { return m_mProjectionMatrix; }

    const maths::CVector3& GetPosition() const { return m_vPos; }
    const maths::CVector3& GetRotation() const { return m_vRot; }

    void SetFov(float _fFov) { m_fFov = _fFov; }
    void SetFar(float _fFar) { m_fFar = _fFar; }
    void SetNear(float _fNear) { m_fNear = _fNear; }

    void SetCameraSpeed(float _fCameraSpeed) { m_fCameraSpeed = _fCameraSpeed; }
    void SetAspectRatio(float _fAspectRatio) { m_fAspectRatio = _fAspectRatio; }

    void SetPosition(const maths::CVector3& pos);
    void SetRotation(const maths::CVector3& rot);

    void MovePosition(const maths::CVector3& pos);
    void AddRotation(const maths::CVector3& rot);
    void SetLookAtPos(const maths::CVector3& _v3LookAtPos);

  private:
    void ShowCursor(bool _bMousePressed);
    void UpdatePerspectiveMatrix();
    void UpdateViewMatrix();

    ConstantBuffer<SConstantMatrix> m_oConstantBuffer;
    maths::CMatrix4x4 m_mViewMatrix = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 m_mProjectionMatrix = maths::CMatrix4x4::Identity;

    maths::CVector3 m_vPos = maths::CVector3::Zero;
    maths::CVector3 m_vRot = maths::CVector3::Zero;

    float m_fCameraSpeed = 20.0f;
    float m_fMovementSpeed = 100.0f;

    float m_fFov = 45.0f;
    float m_fFar = 100000.0f;
    float m_fNear = 0.01f;
    float m_fAspectRatio = 1.777777f;
  };
}

