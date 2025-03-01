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

    void SetPosition(const maths::CVector3& _v3Pos);
    const maths::CVector3& GetPosition() const { return m_vPos; }
    void SetRotation(const maths::CVector3& _v3Rot);
    const maths::CVector3& GetRotation() const { return m_vRot; }

    void SetFov(float _fFov) { m_fFov = _fFov; }
    const float& GetFov() const { return m_fFov; }
    void SetFar(float _fFar) { m_fFar = _fFar; }
    const float& GetFar() const { return m_fFar; }
    void SetNear(float _fNear) { m_fNear = _fNear; }
    const float& GetNear() const { return m_fNear; }

    void SetCameraSpeed(float _fCameraSpeed) { m_fCameraSpeed = _fCameraSpeed; }
    const float& GetCameraSpeed() { return m_fCameraSpeed; }
    void SetAspectRatio(float _fAspectRatio) { m_fAspectRatio = _fAspectRatio; }
    const float& GetAspectRatio() const { return m_fAspectRatio; }

  private:
    void MovePosition(const maths::CVector3& _v3Move);
    void SetLookAtPos(const maths::CVector3& _v3LookAtPos);

    void UpdatePerspectiveMatrix();
    void UpdateViewMatrix();

    void AddRotation(const maths::CVector3& _v3Delta);
    void ShowCursor(bool _bMousePressed);

    CConstantBuffer<SConstantMatrix> m_oConstantBuffer;
    maths::CMatrix4x4 m_mViewMatrix = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 m_mProjectionMatrix = maths::CMatrix4x4::Identity;

    maths::CVector3 m_vPos = maths::CVector3::Zero;
    maths::CVector3 m_vRot = maths::CVector3::Zero;

    float m_fCameraSpeed = 15.0f;
    float m_fMovementSpeed = 50.0f;

    float m_fFov = 45.0f;
    float m_fTargetFov = 45.0f;

    float m_fFar = 100000.0f;
    float m_fNear = 0.01f;
    float m_fAspectRatio = 1.77778f;
  };
}

