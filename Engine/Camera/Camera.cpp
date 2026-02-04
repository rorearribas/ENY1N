#include "Camera.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Engine.h"
#include "Engine/Managers/InputManager.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace internal_camera
  {
    static bool s_bWasRightButtonPressed = false;
    static math::CVector2 s_v2LastPosition = math::CVector2::Zero;

    const float s_fMaxWheelDelta = 0.97f;
    const float s_fOrtographicFactor = 1.0f;
    const float s_fMaxPitch = 90.0f;
  }
  // ------------------------------------
  void CCamera::Update(float _fDeltaTime)
  {
    // Show cursor
    input::CInputManager* pInputManager = input::CInputManager::GetInstance();
    input::CMouse* pMouse = pInputManager->GetMouse();

    bool bRightButtonPressed = pMouse->IsRightButtonPressed();
    ShowCursor(bRightButtonPressed, pMouse->GetMousePosition());

    // Movement + rotation
    if (bRightButtonPressed)
    {
      math::CMatrix4x4 mRot = math::CMatrix4x4::CreateRotation(m_v3Rot);
      math::CVector3 v3Forward = mRot * math::CVector3::Forward;
      math::CVector3 v3Right = mRot * math::CVector3::Right;

      bool bPespectiveMode = GetProjectionMode() == EProjectionMode::PERSPECTIVE;
      math::CVector3 v3HorizontalDir = bPespectiveMode ? v3Forward : math::CVector3::Up;

      if (pInputManager->IsKeyPressed('W') && bRightButtonPressed) { AddDisplacement(v3HorizontalDir * m_fMovementVelocity * _fDeltaTime); }
      if (pInputManager->IsKeyPressed('S') && bRightButtonPressed) { AddDisplacement(-v3HorizontalDir * m_fMovementVelocity * _fDeltaTime); }
      if (pInputManager->IsKeyPressed('D') && bRightButtonPressed) { AddDisplacement(v3Right * m_fMovementVelocity * _fDeltaTime); }
      if (pInputManager->IsKeyPressed('A') && bRightButtonPressed) { AddDisplacement(-v3Right * m_fMovementVelocity * _fDeltaTime); }

      if (bPespectiveMode)
      {
        // Rotation
        float xValue = pMouse->GetMouseDelta().x * m_fCamVelocity;
        float yValue = pMouse->GetMouseDelta().y * m_fCamVelocity;

        // Apply rotation
        AddRotation(math::CVector3(math::Rad2Degrees(yValue), math::Rad2Degrees(xValue), 0.0f));
      }
    }

    // Wheel 
    if (!bRightButtonPressed)
    {
      float fMouseDelta = pMouse->GetMouseWheelDelta();
      fMouseDelta = math::Clamp(fMouseDelta, -internal_camera::s_fMaxWheelDelta, internal_camera::s_fMaxWheelDelta);
      if (GetProjectionMode() == EProjectionMode::ORTOGRAPHIC)
      {
        ApplyOrtographicZoom(fMouseDelta, _fDeltaTime);
      }
    }

    // Update
    if (m_bHasBeenUpdated)
    {
      // Update view matrix
      UpdateViewMatrix(m_eProjectionMode);

      // Update projection matrix
      UpdateProjectionMatrix(GetProjectionMode());

      // Update frustum
      BuildFrustumPlanes();
    }

    // Restore state
    m_bHasBeenUpdated = false;
  }
  // ------------------------------------
  bool CCamera::IsOnFrustum(const collision::CAABB& _oBoundingBox) const
  {
    const math::CVector3& v3Center = _oBoundingBox.GetCenter();
    const math::CVector3& v3HalfExtents = _oBoundingBox.GetHalfSize();

    for (uint32_t uIndex = 0; uIndex < s_uFrustumPlanes; ++uIndex)
    {
      const math::CPlane& rPlane = m_oPlanes[uIndex];
      const math::CVector3& v3Normal = rPlane.GetNormal();

      // Get each component!
      const float fX = std::abs(v3Normal.x);
      const float fY = std::abs(v3Normal.y);
      const float fZ = std::abs(v3Normal.z);

      // Compute test
      const float fDistance = rPlane.DistanceToPoint(v3Center);
      const float fRadius = (v3HalfExtents.x * fX) + (v3HalfExtents.y * fY) + (v3HalfExtents.z * fZ);
      if ((fDistance + fRadius) < 0.0f)
      {
        return false;
      }
    }

    return true;
  }
  // ------------------------------------
  void CCamera::SetPos(const math::CVector3& _v3Pos)
  {
    m_v3Pos = _v3Pos;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::SetRot(const math::CVector3& _v3Rot)
  {
    m_v3Rot = _v3Rot;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::SetFov(float _fFov)
  {
    // Set value
    m_fFov = _fFov;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::SetFar(float _fFar)
  {
    // Set value
    m_fFar = _fFar;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::SetNear(float _fNear)
  {
    // Set value
    m_fNear = _fNear;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::SetProjectionMode(EProjectionMode _eProjectionMode)
  {
    // Reset values
    m_v3Rot = math::CVector3::Zero;
    m_v3Dir = math::CVector3::Forward;
    m_v3Pos.z = 0.0f;

    // Set projection mode
    m_eProjectionMode = _eProjectionMode;

    // Mark as updated
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::LookAt(const math::CVector3& _v3LookAt)
  {
    if (_v3LookAt == m_v3Pos)
    {
      return;
    }

    math::CVector3 v3Dir = math::CVector3::Normalize(_v3LookAt - m_v3Pos);
    float fPitch = static_cast<float>(atan2(v3Dir.y, sqrt(v3Dir.x * v3Dir.x + v3Dir.z * v3Dir.z)));
    float fYaw = static_cast<float>(atan2(v3Dir.x, v3Dir.z));
    if (v3Dir.z > 0)
    {
      fYaw += static_cast<float>(math::s_fPI);
    }

    SetRot(math::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::AddDisplacement(const math::CVector3& _v3Delta)
  {
    m_v3Pos += _v3Delta;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::AddRotation(const math::CVector3& _v3DeltaRot)
  {
    m_v3Rot += _v3DeltaRot;
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::ApplyOrtographicZoom(float fMouseDelta, float _fDeltaTime)
  {
    // Update projection matrix
    m_fZoomScale += fMouseDelta * _fDeltaTime;
    m_fZoomScale = math::Clamp(m_fZoomScale, math::s_fEpsilon7, FLT_MAX);
    m_bHasBeenUpdated = true;
  }
  // ------------------------------------
  void CCamera::UpdateProjectionMatrix(EProjectionMode _eProjectionMode)
  {
    switch (_eProjectionMode)
    {
    case EProjectionMode::PERSPECTIVE:
    {
      m_mProjection = math::CMatrix4x4::CreatePerspectiveMatrix(m_fFov, m_fAspectRatio, m_fNear, m_fFar);
    }
    break;
    case EProjectionMode::ORTOGRAPHIC:
    {
      float fWidth = internal_camera::s_fOrtographicFactor / m_fZoomScale;
      float fHeight = (internal_camera::s_fOrtographicFactor / m_fAspectRatio) / m_fZoomScale;
      m_mProjection = math::CMatrix4x4::CreateOrtographicMatrix(fWidth, fHeight, m_fNear, m_fFar);
    }
    break;
    }
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix(EProjectionMode _eProjectionMode)
  {
    math::CVector3 v3Up = render::CRender::s_v3WorldUp; // Default World Up
    math::CVector3 v3TargetPos = m_v3Pos + m_v3Dir; // Default Offset

    if (_eProjectionMode == EProjectionMode::PERSPECTIVE)
    {
      // Clamp pitch value
      m_v3Rot.x = math::Clamp(m_v3Rot.x, -internal_camera::s_fMaxPitch, internal_camera::s_fMaxPitch);

      // Create rotation matrix
      math::CMatrix4x4 mRot = math::CMatrix4x4::CreateRotation(m_v3Rot);

      // Calculate dir
      m_v3Dir = mRot * math::CVector3::Forward;
      v3TargetPos = m_v3Pos + m_v3Dir;

      // Calculate up direction
      v3Up = mRot * math::CVector3::Up;
    }

    // Set view matrix
    m_mViewMatrix = math::CMatrix4x4::LookAt(m_v3Pos, v3TargetPos, v3Up);
  }
  // ------------------------------------
  void CCamera::DrawDebug()
  {
    ImGui::Begin("CAMERA");

    const math::CVector3& v3CurrentPos = GetPos();
    float fPos[3] = { v3CurrentPos.x, v3CurrentPos.y, v3CurrentPos.z };
    ImGui::InputFloat3("Position", fPos);
    {
      math::CVector3 v3TargetPos(fPos[0], fPos[1], fPos[2]);
      if (v3TargetPos != v3CurrentPos)
      {
        SetPos(v3TargetPos);
      }
    }

    const math::CVector3& v3CurrentRot = GetRot();
    float fRot[3] = { v3CurrentRot.x, v3CurrentRot.y, v3CurrentRot.z };
    ImGui::InputFloat3("Rotation", fRot);
    {
      math::CVector3 v3TargetRot(fRot[0], fRot[1], fRot[2]);
      if (v3TargetRot != v3CurrentRot)
      {
        SetRot(v3TargetRot);
      }
    }

    const math::CVector3& v3CurrentDir = GetDir();
    float fDir[3] = { v3CurrentDir.x, v3CurrentDir.y, v3CurrentDir.z };
    ImGui::BeginDisabled();
    ImGui::InputFloat3("Direction", fDir);
    ImGui::EndDisabled();

    ImGui::Separator();
    float fTargetFov = GetFov();
    ImGui::InputFloat("FOV", &fTargetFov);
    if (fTargetFov != GetFov())
    {
      SetFov(fTargetFov);
    }

    ImGui::Separator();
    float fTargetVel = GetMovementVel();
    ImGui::InputFloat("Velocity", &fTargetVel);
    if (fTargetVel != GetMovementVel())
    {
      SetMovementVel(fTargetVel);
    }

    if (ImGui::Button("Perspective Mode"))
    {
      SetProjectionMode(render::EProjectionMode::PERSPECTIVE);
    }
    if (ImGui::Button("Orthographic Mode"))
    {
      SetProjectionMode(render::EProjectionMode::ORTOGRAPHIC);
    }

    ImGui::End();
  }
  // ------------------------------------
  void CCamera::BuildFrustumPlanes()
  {
    // Get tangent
    float fFovRadians = math::Deg2Radians(m_fFov);
    float fTan = tanf(fFovRadians / 2.0f);

    // Get half height and width
    float fHalfVSide = fTan * m_fFar;
    float fHalfHSide = fHalfVSide * m_fAspectRatio;
    const math::CVector3 v3NearDir = m_v3Dir * m_fNear;
    const math::CVector3 v3FarDir = m_v3Dir * m_fFar;

    // Get axis
    math::CVector3 v3Right = math::CVector3::Normalize(math::CVector3::Cross(math::CVector3::Up, m_v3Dir));
    math::CVector3 v3Up = math::CVector3::Normalize(math::CVector3::Cross(m_v3Dir, v3Right));

    math::CVector3 v3FarRight = v3FarDir + (v3Right * fHalfHSide);
    math::CVector3 v3FarLeft = v3FarDir - (v3Right * fHalfHSide);
    math::CVector3 v3FarTop = v3FarDir + (v3Up * fHalfVSide);
    math::CVector3 v3FarBottom = v3FarDir - (v3Up * fHalfVSide);

    m_oPlanes[0].Set(m_v3Pos + v3NearDir, m_v3Dir); // Near
    m_oPlanes[1].Set(m_v3Pos + v3FarDir, -m_v3Dir); // Far
    m_oPlanes[2].Set(m_v3Pos, math::CVector3::Cross(v3FarRight, v3Up)); // Right
    m_oPlanes[3].Set(m_v3Pos, math::CVector3::Cross(v3Up, v3FarLeft)); // Left
    m_oPlanes[4].Set(m_v3Pos, math::CVector3::Cross(v3Right, v3FarTop));  // Top
    m_oPlanes[5].Set(m_v3Pos, math::CVector3::Cross(v3FarBottom, v3Right)); // Bottom
  }
  // ------------------------------------
  void CCamera::ShowCursor(bool _bMousePressed, const math::CVector2& _vMousePos)
  {
    if (!internal_camera::s_bWasRightButtonPressed && _bMousePressed)
    {
      internal_camera::s_v2LastPosition = _vMousePos;
      internal_camera::s_bWasRightButtonPressed = true;
    }
    else if (internal_camera::s_bWasRightButtonPressed && !_bMousePressed)
    {
      int iLastX = static_cast<int>(internal_camera::s_v2LastPosition.x);
      int iLastY = static_cast<int>(internal_camera::s_v2LastPosition.y);
      SetCursorPos(iLastX, iLastY);
      internal_camera::s_bWasRightButtonPressed = false;
    }

    while (_bMousePressed ? ::ShowCursor(!_bMousePressed) >= 0 : ::ShowCursor(!_bMousePressed) < 0);
  }
}
