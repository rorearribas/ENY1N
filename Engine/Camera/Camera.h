#pragma once
#include "Engine/Render/ConstantBuffer/BufferTypes.h"
#include "Engine/Collisions/AABB.h"
#include "Engine/Utils/Plane.h"

#include "Libs/Math/Matrix4x4.h"
#include "Libs/Math/Vector3.h"
#include "Libs/Math/Vector2.h"

namespace render
{
  enum class EProjectionMode
  {
    PERSPECTIVE,
    ORTOGRAPHIC
  };

  class CCamera
  {
  private:
    static constexpr uint32_t s_uFrustumPlanes = 6;

  public:
    CCamera() {}
    ~CCamera() {}

    void Update(float _fDeltaTime);
    bool IsOnFrustum(const collision::CAABB& _oBoundingBox) const;

    const math::CMatrix4x4& GetViewMatrix() const { return m_mViewMatrix; }
    const math::CMatrix4x4& GetProjectionMatrix() const { return m_mProjection; }
    math::CMatrix4x4 GetViewProjection() const { return m_mProjection * m_mViewMatrix; }

    inline const math::CVector3& GetCameraDir() const { return m_v3Dir; }
    inline void SetPosition(const math::CVector3& _v3Pos) { m_v3Pos = _v3Pos; UpdateViewMatrix(GetProjectionMode()); }
    const math::CVector3& GetPosition() const { return m_v3Pos; }
    inline void SetRotation(const math::CVector3& _v3Rot) { m_v3Rot = _v3Rot; UpdateViewMatrix(GetProjectionMode()); }
    inline const math::CVector3& GetRotation() const { return m_v3Rot; }

    inline void SetMovementVel(float _fVelocity) { m_fMovementVelocity = _fVelocity; }
    inline float GetMovementVel() const { return m_fMovementVelocity; }
    inline void SetFov(float _fFov) { m_fFov = _fFov; }
    inline float GetFov() const { return m_fFov; }
    inline void SetFar(float _fFar) { m_fFar = _fFar; }
    inline float GetFar() const { return m_fFar; }
    inline void SetNear(float _fNear) { m_fNear = _fNear; }
    inline float GetNear() const { return m_fNear; }

    void SetProjectionMode(EProjectionMode _eProjectionMode);
    inline const EProjectionMode& GetProjectionMode() const { return m_eProjectionMode; }

    inline void SetCamVelocity(float _fCameraSpeed) { m_fCamVelocity = _fCameraSpeed; }
    inline float GetCamVelocity() const { return m_fCamVelocity; }
    inline void SetAspectRatio(float _fAspectRatio) { m_fAspectRatio = _fAspectRatio; }
    inline float GetAspectRatio() const { return m_fAspectRatio; }

    void DrawDebug();

  private:
    void LookAt(const math::CVector3& _v3LookAtPos);
    void AddDisplacement(const math::CVector3& _v3Delta);
    void AddRotation(const math::CVector3& _v3Delta);

    void UpdateProjectionMatrix(EProjectionMode _eProjectionMode);
    void UpdateViewMatrix(EProjectionMode _eProjectionMode);
    void BuildFrustumPlanes();

    void ShowCursor(bool _bMousePressed, const math::CVector2& _vMousePos);

    math::CMatrix4x4 m_mViewMatrix = math::CMatrix4x4::Identity;
    math::CMatrix4x4 m_mProjection = math::CMatrix4x4::Identity;

    math::CVector3 m_v3Dir = math::CVector3::Zero;
    math::CVector3 m_v3Pos = math::CVector3::Zero;
    math::CVector3 m_v3Rot = math::CVector3::Zero;

    float m_fCamVelocity = 1.0f;
    float m_fMovementVelocity = 25.0f;

    float m_fFov = 45.0f;
    float m_fZoomScale = 0.01f;

    float m_fFar = 10000.0f;
    float m_fNear = 0.01f;
    float m_fAspectRatio = 1.77778f;

    math::CPlane m_oPlanes[s_uFrustumPlanes];
    EProjectionMode m_eProjectionMode = EProjectionMode::PERSPECTIVE;
  };
}

