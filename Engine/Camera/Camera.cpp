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

    const int s_iFrustumPlanes = 6;
    const float s_fInterpolateSpeed = 10.0f;
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
    math::CVector3 vForward = math::CVector3::Normalize(mRotMatrix * math::CVector3::Forward);
    math::CVector3 vRight = math::CVector3::Normalize(mRotMatrix * math::CVector3::Right);

    // Show cursor
    input::CInputManager* pInputManager = input::CInputManager::GetInstance();
    input::CMouse* pMouse = pInputManager->GetMouse();

    bool bRightButtonPressed = pMouse->IsRightButtonPressed();
    ShowCursor(bRightButtonPressed, pMouse->GetMousePosition());

    // Movement + rotation
    if (bRightButtonPressed)
    {
      if (pInputManager->IsKeyPressed('W') && bRightButtonPressed) MovePosition(vForward * m_fMovementSpeed * _fDeltaTime);
      if (pInputManager->IsKeyPressed('S') && bRightButtonPressed) MovePosition(-vForward * m_fMovementSpeed * _fDeltaTime);
      if (pInputManager->IsKeyPressed('D') && bRightButtonPressed) MovePosition(vRight * m_fMovementSpeed * _fDeltaTime);
      if (pInputManager->IsKeyPressed('A') && bRightButtonPressed) MovePosition(-vRight * m_fMovementSpeed * _fDeltaTime);

      // Rotation
      float xValue = pMouse->GetMouseDelta().X * m_fCameraSpeed;
      float yValue = pMouse->GetMouseDelta().Y * m_fCameraSpeed;

      // Apply rotation
      AddRotation(math::CVector3(math::Rad2Degrees(yValue), math::Rad2Degrees(xValue), 0.0f));
    }

    // Wheel 
    if (!bRightButtonPressed)
    {
      float fMouseDelta = pMouse->GetMouseWheelDelta();
      MovePosition(fMouseDelta != 0 ? vForward * (fMouseDelta * m_fMovementSpeed) * _fDeltaTime : math::CVector3::Zero);
    }

    // Interpolate FOV
    m_fDesiredFov = math::Lerp(m_fDesiredFov, m_fFov, internal_camera::s_fInterpolateSpeed * _fDeltaTime);

    // Update perspective matrix
    UpdatePerspectiveMatrix();

    // Update frustum
    BuildFrustumPlanes();

    // Update constant buffer
    m_oConstantBuffer.GetData().Matrix = math::CMatrix4x4::Transpose(m_mViewMatrix * m_mProjectionMatrix);
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
  void CCamera::MovePosition(const math::CVector3& vDelta) 
  {
    m_v3Pos += vDelta;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AddRotation(const math::CVector3& _vDeltaRot) 
  {
    m_v3Rot += _vDeltaRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetLookAtPos(const math::CVector3& _v3LookAt) 
  {
    if (_v3LookAt == m_v3Pos)
      return;

    math::CVector3 v3Dir = _v3LookAt - m_v3Pos;
    float fPitch = (float)atan2(v3Dir.Y, sqrt(v3Dir.X * v3Dir.X + v3Dir.Z * v3Dir.Z));
    float fYaw = (float)atan2(v3Dir.X, v3Dir.Z);
    if (v3Dir.Z > 0) fYaw += static_cast<float>(math::s_fPI);

    SetRotation(math::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::UpdatePerspectiveMatrix()
  {
    if (m_eProjectionMode == PERSPECTIVE)
    {
      m_mProjectionMatrix = math::CMatrix4x4::CreatePerspectiveMatrix(m_fDesiredFov, m_fAspectRatio, m_fNear, m_fFar);
    }
    else
    {
      render::CRender* pRender = engine::CEngine::GetInstance()->GetRender();
      render::CRenderWindow* pRenderWindow = pRender->GetRenderWindow();
      float fWidth = static_cast<float>(pRenderWindow->GetWidth());
      float fHeight = static_cast<float>(pRenderWindow->GetHeight());
      m_mProjectionMatrix = math::CMatrix4x4::CreateOrtographicMatrix(fWidth, fHeight, m_fNear, m_fFar);
    }
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix()
  {
    math::CVector3 v3Up = math::CVector3::Up;
    m_v3Dir = math::CVector3::Forward;
    math::CVector3 v3TargetPos = m_v3Pos + m_v3Dir;

    if (m_eProjectionMode == PERSPECTIVE)
    {
      // Clamp pitch value
      m_v3Rot.X = math::Clamp(m_v3Rot.X, -90.0f, 90.0f);

      // Create the rotation matrix
      math::CMatrix4x4 mRotationMatrix = math::CMatrix4x4::Rotation(m_v3Rot);

      // Calculate dir
      m_v3Dir = math::CVector3::Normalize(mRotationMatrix * math::CVector3::Forward);
      v3TargetPos = m_v3Pos + m_v3Dir;

      // Calculate up direction
      v3Up = mRotationMatrix * math::CVector3::Up;
    }

    // Set view matrix
    m_mViewMatrix = math::CMatrix4x4::LookAt(m_v3Pos, v3TargetPos, v3Up);
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
  // ------------------------------------
  void CCamera::DrawFrustum(const math::CVector3& /*v3Origin*/)
  {

  }
}
