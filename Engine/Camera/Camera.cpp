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
    // Keyboard movement
    math::CMatrix4x4 mRotMatrix = math::CMatrix4x4::Rotation(m_v3Rot);
    math::CVector3 v3Forward = mRotMatrix * math::CVector3::Forward;
    math::CVector3 v3Right = mRotMatrix * math::CVector3::Right;

    // Show cursor
    input::CInputManager* pInputManager = input::CInputManager::GetInstance();
    input::CMouse* pMouse = pInputManager->GetMouse();

    bool bRightButtonPressed = pMouse->IsRightButtonPressed();
    ShowCursor(bRightButtonPressed, pMouse->GetMousePosition());

    // Movement + rotation
    if (bRightButtonPressed)
    {
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
      if (GetProjectionMode() == EProjectionMode::PERSPECTIVE)
      {
        AddDisplacement(fMouseDelta != 0 ? v3Forward * (fMouseDelta * m_fMovementVelocity) * _fDeltaTime : math::CVector3::Zero);
      }
      else
      {
        m_fZoomScale += fMouseDelta * _fDeltaTime;
        m_fZoomScale = math::Clamp(m_fZoomScale, math::s_fEpsilon7, FLT_MAX);
      }
    }

    //@Note i should change this!!
    UpdateProjectionMatrix(GetProjectionMode());

    // Update frustum
    BuildFrustumPlanes();
  }
  // ------------------------------------
  bool CCamera::IsOnFrustum(const collision::CBoundingBox& _oBoundingBox)
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
  void CCamera::SetProjectionMode(EProjectionMode _eProjectionMode)
  {
    // Reset values
    m_v3Rot = math::CVector3::Zero;
    m_v3Dir = math::CVector3::Forward;
    m_v3Pos.z = 0.0f;

    // Update projection matrix
    UpdateProjectionMatrix(_eProjectionMode);

    // Update view matrix
    UpdateViewMatrix(_eProjectionMode);

    // Set projection mode
    m_eProjectionMode = _eProjectionMode;
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

    SetRotation(math::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::AddDisplacement(const math::CVector3& _v3Delta)
  {
    m_v3Pos += _v3Delta;
    UpdateViewMatrix(m_eProjectionMode);
  }
  // ------------------------------------
  void CCamera::AddRotation(const math::CVector3& _v3DeltaRot)
  {
    m_v3Rot += _v3DeltaRot;
    UpdateViewMatrix(m_eProjectionMode);
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
      math::CMatrix4x4 mRotMatrix = math::CMatrix4x4::Rotation(m_v3Rot);

      // Calculate dir
      m_v3Dir = mRotMatrix * math::CVector3::Forward;
      v3TargetPos = m_v3Pos + m_v3Dir;

      // Calculate up direction
      v3Up = mRotMatrix * math::CVector3::Up;
    }

    // Set view matrix
    m_mViewMatrix = math::CMatrix4x4::LookAt(m_v3Pos, v3TargetPos, v3Up);
  }
  // ------------------------------------
  void CCamera::DrawDebug()
  {
    ImGui::Begin("CAMERA");
    const math::CVector3& v3Position = GetPosition();
    float camera_pos[3] = { v3Position.x, v3Position.y, v3Position.z };
    ImGui::InputFloat3("Position", camera_pos);
    SetPosition(math::CVector3(camera_pos[0], camera_pos[1], camera_pos[2]));

    const math::CVector3& v3Rot = GetRotation();
    float camera_rot[3] = { v3Rot.x, v3Rot.y, v3Rot.z };
    ImGui::InputFloat3("Rotation", camera_rot);
    SetRotation(math::CVector3(camera_rot[0], camera_rot[1], camera_rot[2]));

    const math::CVector3& v3Dir = GetCameraDir();
    float camera_dir[3] = { v3Dir.x, v3Dir.y, v3Dir.z };
    ImGui::InputFloat3("Direction", camera_dir);

    ImGui::Separator();
    float fFov = GetFov();
    ImGui::InputFloat("FOV", &fFov);
    SetFov(fFov);

    ImGui::Separator();
    float fVel = GetMovementVel();
    ImGui::InputFloat("Velocity", &fVel);
    SetMovementVel(fVel);

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
