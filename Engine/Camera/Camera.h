#pragma once
#include "Engine/Render/ConstantBuffer/ConstantBuffer.h"
#include "Engine/Utils/Plane.h"
#include "Libs/Math/Matrix4x4.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"

namespace render
{
  class CCamera
  {
  public:
    CCamera();
    ~CCamera() {}

    void Update(float _fDeltaTime);

    const math::CVector3& GetDir() { return m_v3Dir; }
    const math::CMatrix4x4& GetViewMatrix() const { return m_mViewMatrix; }
    const math::CMatrix4x4& GetProjectionMatrix() const { return m_mProjectionMatrix; }

    void SetPosition(const math::CVector3& _v3Pos);
    const math::CVector3& GetPosition() const { return m_v3Pos; }
    void SetRotation(const math::CVector3& _v3Rot);
    const math::CVector3& GetRotation() const { return m_v3Rot; }

    void SetFov(float _fFov) { m_fFov = _fFov; }
    const float& GetFov() const { return m_fFov; }
    void SetFar(float _fFar) { m_fFar = _fFar; }
    const float& GetFar() const { return m_fFar; }
    void SetNear(float _fNear) { m_fNear = _fNear; }
    const float& GetNear() const { return m_fNear; }

    void SetCameraSpeed(float _fCameraSpeed) { m_fCameraSpeed = _fCameraSpeed; }
    const float& GetCameraSpeed() const { return m_fCameraSpeed; }
    void SetAspectRatio(float _fAspectRatio) { m_fAspectRatio = _fAspectRatio; }
    const float& GetAspectRatio() const { return m_fAspectRatio; }

  private:
    void MovePosition(const math::CVector3& _v3Move);
    void SetLookAtPos(const math::CVector3& _v3LookAtPos);

    void UpdatePerspectiveMatrix();
    void UpdateViewMatrix();
    void UpdateFrustum();

    void AddRotation(const math::CVector3& _v3Delta);
    void ShowCursor(bool _bMousePressed, const math::CVector2& _vMousePos);

    CConstantBuffer<SConstantMatrix> m_oConstantBuffer;
    math::CMatrix4x4 m_mViewMatrix = math::CMatrix4x4::Identity;
    math::CMatrix4x4 m_mProjectionMatrix = math::CMatrix4x4::Identity;

    math::CVector3 m_v3Dir = math::CVector3::Zero;
    math::CVector3 m_v3Pos = math::CVector3::Zero;
    math::CVector3 m_v3Rot = math::CVector3::Zero;

    float m_fCameraSpeed = 1.0f;
    float m_fMovementSpeed = 50.0f;

    float m_fFov = 45.0f;
    float m_fTargetFov = 45.0f;

    float m_fFar = 100000.0f;
    float m_fNear = 0.01f;
    float m_fAspectRatio = 1.77778f;

    std::vector<math::CPlane> m_oFrustumPlanes;
  };
}

