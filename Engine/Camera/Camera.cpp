#include "Camera.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Base/Engine.h"
#include "Engine/Managers/InputManager.h"
#include "Libs/Math/Math.h"
#include "Libs/Macros/GlobalMacros.h"
#include "../Collisions/CollisionManager.h"

namespace render
{
  namespace internal_camera
  {
    static bool s_bWasRightButtonPressed = false;
    static math::CVector2 s_v2LastPosition = math::CVector2::Zero;

    const float s_fInterpolateSpeed = 10.0f;
    const int s_iFrustumPlanes = 6;

    const float s_fMaxWheelDelta = 0.97f;
    const float s_fOrtographicFactor = 1.0f;
    const float s_fMaxPitch = 90.0f;
  }
  // ------------------------------------
  CCamera::CCamera()
  {
    m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    m_oFrustumPlanes.resize(internal_camera::s_iFrustumPlanes);
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
        float xValue = pMouse->GetMouseDelta().X * m_fCamVelocity;
        float yValue = pMouse->GetMouseDelta().Y * m_fCamVelocity;

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

    // Interpolate FOV
    m_fDesiredFov = math::Lerp(m_fDesiredFov, m_fFov, internal_camera::s_fInterpolateSpeed * _fDeltaTime);

    // Update projection matrix
    UpdateProjectionMatrix(GetProjectionMode());

    // Update frustum
    BuildFrustumPlanes();

    // Update constant buffer
    m_oConstantBuffer.GetData().Matrix =  m_mProjection * m_mViewMatrix;
    bool bOk = m_oConstantBuffer.WriteBuffer();
    UNUSED_VAR(bOk);
    assert(bOk);

    ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
    global::dx11::s_pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
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
      int iLastX = static_cast<int>(internal_camera::s_v2LastPosition.X);
      int iLastY = static_cast<int>(internal_camera::s_v2LastPosition.Y);
      SetCursorPos(iLastX, iLastY);
      internal_camera::s_bWasRightButtonPressed = false;
    }

    while (_bMousePressed ? ::ShowCursor(!_bMousePressed) >= 0 : ::ShowCursor(!_bMousePressed) < 0);
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
  void CCamera::LookAt(const math::CVector3& _v3LookAt)
  {
    if (_v3LookAt == m_v3Pos)
    {
      return;
    }

    math::CVector3 v3Dir = math::CVector3::Normalize(_v3LookAt - m_v3Pos);
    float fPitch = static_cast<float>(atan2(v3Dir.Y, sqrt(v3Dir.X * v3Dir.X + v3Dir.Z * v3Dir.Z)));
    float fYaw = static_cast<float>(atan2(v3Dir.X, v3Dir.Z));
    if (v3Dir.Z > 0) 
    {
      fYaw += static_cast<float>(math::s_fPI);
    }

    SetRotation(math::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::SetProjectionMode(EProjectionMode _eProjectionMode)
  {
    // Reset values
    m_v3Rot = math::CVector3::Zero;
    m_v3Dir = math::CVector3::Forward;
    m_v3Pos.Z = 0.0f;

    // Update view matrix
    UpdateViewMatrix(_eProjectionMode);

    // Set projection mode
    m_eProjectionMode = _eProjectionMode;
  }
  // ------------------------------------
  void CCamera::UpdateProjectionMatrix(EProjectionMode _eProjectionMode)
  {
    switch (_eProjectionMode) 
    {
      case EProjectionMode::PERSPECTIVE:
      {
        m_mProjection = math::CMatrix4x4::CreatePerspectiveMatrix(m_fDesiredFov, m_fAspectRatio, m_fNear, m_fFar);
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
      m_v3Rot.X = math::Clamp(m_v3Rot.X, -internal_camera::s_fMaxPitch, internal_camera::s_fMaxPitch);

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
    float camera_pos[3] = { v3Position.X, v3Position.Y, v3Position.Z };
    ImGui::InputFloat3("Position", camera_pos);
    SetPosition(math::CVector3(camera_pos[0], camera_pos[1], camera_pos[2]));

    const math::CVector3& v3Rot = GetRotation();
    float camera_rot[3] = { v3Rot.X, v3Rot.Y, v3Rot.Z };
    ImGui::InputFloat3("Rotation", camera_rot);
    SetRotation(math::CVector3(camera_rot[0], camera_rot[1], camera_rot[2]));

    const math::CVector3& v3Dir = GetCameraDir();
    float camera_dir[3] = { v3Dir.X, v3Dir.Y, v3Dir.Z };
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
      SetProjectionMode(render::CCamera::EProjectionMode::PERSPECTIVE);
    }
    if (ImGui::Button("Ortographic Mode"))
    {
      SetProjectionMode(render::CCamera::EProjectionMode::ORTOGRAPHIC);
    }
    ImGui::End();
  }
  // ------------------------------------
  void CCamera::BuildFrustumPlanes()
  {
    float fFovRadians = math::Deg2Radians(m_fDesiredFov);
    float fTan = tanf(fFovRadians / 2.0f);

    // Far plane
    float fHalfHeight = fTan * m_fFar;
    float fHalfWidth = fHalfHeight * m_fAspectRatio;

    // Get axis
    math::CVector3 v3Right = math::CVector3::Cross(math::CVector3::Up, m_v3Dir).Normalize();
    math::CVector3 v3Up = math::CVector3::Cross(m_v3Dir, v3Right).Normalize();

    math::CVector3 v3FrontFar = m_v3Dir * m_fFar;
    math::CVector3 v3FrontNear = m_v3Dir * m_fNear;

    math::CVector3 v3FarPos = m_v3Pos + v3FrontFar;
    math::CVector3 v3NearPos = m_v3Pos + v3FrontNear;

    m_oFrustumPlanes[0].Set(v3NearPos, m_v3Dir); // Near
    m_oFrustumPlanes[1].Set(v3FarPos, -m_v3Dir); // Far
    m_oFrustumPlanes[2].Set(v3NearPos, math::CVector3::Cross(v3FrontFar - v3Right * fHalfHeight, v3Up).Normalize()); // Right
    m_oFrustumPlanes[3].Set(v3NearPos, math::CVector3::Cross(v3Up, v3FrontFar + v3Right * fHalfHeight).Normalize()); // Left
    m_oFrustumPlanes[4].Set(v3NearPos, math::CVector3::Cross(v3Right, v3FrontFar - v3Up * fHalfWidth).Normalize()); // Top
    m_oFrustumPlanes[5].Set(v3NearPos, math::CVector3::Cross(v3FrontFar + v3Up * fHalfWidth, v3Right).Normalize()); // Bottom

    //// Planes
    //math::CVector3 v3CenterNear = m_v3Pos + m_v3Dir * m_fNear;
    //math::CVector3 v3CenterFar = m_v3Pos + m_v3Dir * m_fFar;

    //// Near edges
    //math::CVector3 v3NearTopLeft = v3CenterNear + (v3Up * (fHalfWidth * 0.5f)) - (v3Right * (fHalfWidth * 0.5f));
    //math::CVector3 v3NearTopRight = v3CenterNear + (v3Up * (fHalfWidth * 0.5f)) + (v3Right * (fHalfWidth * 0.5f));
    //math::CVector3 v3NearBottomLeft = v3CenterNear - (v3Up * (fHalfWidth * 0.5f)) - (v3Right * (fHalfWidth * 0.5f));
    //math::CVector3 v3NearBottomRight = v3CenterNear - (v3Up * (fHalfWidth * 0.5f)) + (v3Right * (fHalfWidth * 0.5f));

    //// Far edges
    //math::CVector3 v3FarTopLeft = v3CenterFar + (v3Up * (fHalfHeight * 0.5f)) - (v3Right * (fHalfHeight * 0.5f));
    //math::CVector3 v3FarTopRight = v3CenterFar + (v3Up * (fHalfHeight * 0.5f)) + (v3Right * (fHalfHeight * 0.5f));
    //math::CVector3 v3FarBottomLeft = v3CenterFar - (v3Up * (fHalfHeight * 0.5f)) - (v3Right * (fHalfHeight * 0.5f));
    //math::CVector3 v3FarBottomRight = v3CenterFar - (v3Up * (fHalfHeight * 0.5f)) + (v3Right * (fHalfHeight * 0.5f));

    //// Near plane
    //engine::CEngine* pEngine = engine::CEngine::GetInstance();
    //pEngine->DrawLine(v3NearTopLeft, v3NearTopRight, math::CVector3::Right);
    //pEngine->DrawLine(v3NearTopRight, v3NearBottomRight, math::CVector3::Right);
    //pEngine->DrawLine(v3NearBottomRight, v3NearBottomLeft, math::CVector3::Right);
    //pEngine->DrawLine(v3NearBottomLeft, v3NearTopLeft, math::CVector3::Right);

    //// Far plane
    //pEngine->DrawLine(v3FarTopLeft, v3FarTopRight, math::CVector3::Up);
    //pEngine->DrawLine(v3FarTopRight, v3FarBottomRight, math::CVector3::Up);
    //pEngine->DrawLine(v3FarBottomRight, v3FarBottomLeft, math::CVector3::Up);
    //pEngine->DrawLine(v3FarBottomLeft, v3FarTopLeft, math::CVector3::Up);

    //// Connect near and far planes
    //pEngine->DrawLine(v3NearTopLeft, v3FarTopLeft, math::CVector3::Forward);
    //pEngine->DrawLine(v3NearTopRight, v3FarTopRight, math::CVector3::Forward);
    //pEngine->DrawLine(v3NearBottomLeft, v3FarBottomLeft, math::CVector3::Forward);
    //pEngine->DrawLine(v3NearBottomRight, v3FarBottomRight, math::CVector3::Forward);

    //pEngine->DrawLine(m_v3Pos, m_v3Pos + v3NearTopLeft, math::CVector3::Forward);
    //pEngine->DrawLine(m_v3Pos, m_v3Pos + v3NearTopRight, math::CVector3::Forward);
    //pEngine->DrawLine(m_v3Pos, m_v3Pos + v3NearBottomLeft, math::CVector3::Forward);
    //pEngine->DrawLine(m_v3Pos, m_v3Pos + v3NearBottomRight, math::CVector3::Forward);
  }
}
