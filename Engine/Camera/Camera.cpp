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
    const int s_iFrustumPlanes = 6;
    const float s_fInterpolateSpeed = 10.0f;

    static bool s_bWasRightButtonPressed = false;
    static math::CVector2 s_v2LastPosition = math::CVector2::Zero;
  }
  // ------------------------------------
  CCamera::CCamera()
  {
    m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
    m_oFrustumPlanes.reserve(internal_camera::s_iFrustumPlanes);
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

      math::CVector3 v3AddedRot(math::RadiansToDegrees(yValue), math::RadiansToDegrees(xValue), 0.0f);
      AddRotation(v3AddedRot);
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
    UpdateFrustum();

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
  void CCamera::SetPosition(const math::CVector3& newPos) 
  {
    m_v3Pos = newPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::MovePosition(const math::CVector3& vDelta) 
  {
    m_v3Pos += vDelta;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(const math::CVector3& _vNewRot) 
  {
    m_v3Rot = _vNewRot;
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
    m_mProjectionMatrix = math::CMatrix4x4::CreatePerspectiveMatrix(m_fDesiredFov, m_fAspectRatio, m_fNear, m_fFar);
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix()
  {
    // Clamp pitch value
    m_v3Rot.X = math::Clamp(m_v3Rot.X, -90.0f, 90.0f);

    // Create the rotation matrix
    math::CMatrix4x4 mRotationMatrix = math::CMatrix4x4::Rotation(m_v3Rot);

    // Calculate dir
    m_v3Dir = math::CVector3::Normalize(mRotationMatrix * math::CVector3::Forward);
    math::CVector3 v3TargetPos = m_v3Pos + m_v3Dir;

    // Calculate up direction
    math::CVector3 vUpDir = mRotationMatrix * math::CVector3::Up;

    // Set view matrix
    m_mViewMatrix = math::CMatrix4x4::LookAt(m_v3Pos, v3TargetPos, vUpDir);
  }
  // ------------------------------------
  void CCamera::UpdateFrustum()
  {
    //float fFovRadians = math::DegreesToRadians(m_fDesiredFov);
    //float fTan = tanf(fFovRadians / 2.0f);

    //// Far plane
    //float fFarHeight = (fTan * m_fFar) * 2.0f;
    //float fFarWidth = fFarHeight * m_fAspectRatio;

    //// Near plane
    //float fNearHeight = (fTan * m_fNear) * 2.0f;
    //float fNearWidth = fNearHeight * m_fAspectRatio;

    //// Top
    //math::CVector3 v3FarTopLeft = { -fFarWidth * 0.5f,  fFarHeight * 0.5f, fTestFar };
    //math::CVector3 v3FarTopRight = { fFarWidth * 0.5f,  fFarHeight * 0.5f, fTestFar };
    //math::CVector3 v3FarBottomLeft = { -fFarWidth * 0.5f, -fFarHeight * 0.5f, fTestFar };
    //math::CVector3 v3FarBottomRight = { fFarWidth * 0.5f, -fFarHeight * 0.5f, fTestFar };

    //// Near
    //math::CVector3 v3NearTopLeft = { -fNearWidth * 0.5f,  fNearHeight * 0.5f, fTestNear };
    //math::CVector3 v3NearTopRight = { fNearWidth * 0.5f,  fNearHeight * 0.5f, fTestNear };
    //math::CVector3 v3NearBottomLeft = { -fNearWidth * 0.5f, -fNearHeight * 0.5f, fTestNear };
    //math::CVector3 v3NearBottomRight = { fNearWidth * 0.5f, -fNearHeight * 0.5f, fTestNear };

    // Draw near
    //engine::CEngine* pEngine = engine::CEngine::GetInstance();
    //math::CVector3 v3Origin(0.0f, 20.0f, 0.0f);

    //// Get valid axis
    //math::CVector3 v3Right = math::CVector3::Cross(math::CVector3::Up, m_v3Dir);
    //v3Right.Normalize();

    //math::CVector3 v3Up = math::CVector3::Cross(m_v3Dir, v3Right);
    //v3Up.Normalize();

    //math::CVector3 v3CenterNear = m_v3Pos + m_v3Dir * m_fNear;
    //math::CVector3 v3NearTopLeft = v3CenterNear + v3Up * (fNearHeight * 0.5f) - v3Right * (fNearWidth * 0.5f);
    //math::CVector3 v3NearTopRight = v3CenterNear + v3Up * (fNearHeight * 0.5f) + v3Right * (fNearWidth * 0.5f);
    //math::CVector3 v3NearBottomLeft = v3CenterNear - v3Up * (fNearHeight * 0.5f) - v3Right * (fNearWidth * 0.5f);
    //math::CVector3 v3NearBottomRight = v3CenterNear - v3Up * (fNearHeight * 0.5f) + v3Right * (fNearWidth * 0.5f);

    //// Far
    //math::CVector3 v3CenterFar = m_v3Pos + m_v3Dir * m_fFar;
    //math::CVector3 v3FarTopLeft = v3CenterFar + v3Up * (fFarHeight * 0.5f) - v3Right * (fFarWidth * 0.5f);
    //math::CVector3 v3FarTopRight = v3CenterFar + v3Up * (fFarHeight * 0.5f) + v3Right * (fFarWidth * 0.5f);
    //math::CVector3 v3FarBottomLeft = v3CenterFar - v3Up * (fFarHeight * 0.5f) - v3Right * (fFarWidth * 0.5f);
    //math::CVector3 v3FarBottomRight = v3CenterFar - v3Up * (fFarHeight * 0.5f) + v3Right * (fFarWidth * 0.5f);

    //// Near plane
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

    //// Draw near plane
    //pEngine->DrawPlane(math::CPlane(m_v3Pos + (math::CVector3::Forward * fTestNear), math::CVector3::Forward),
    //math::CVector3(fNearWidth, fNearHeight, 1.0f), math::CVector3::Forward);

    //// Draw far
    //pEngine->DrawLine(m_v3Pos + v3NearTopLeft, m_v3Pos + v3FarTopLeft, math::CVector3::Right);
    //pEngine->DrawLine(m_v3Pos + v3NearTopRight, m_v3Pos + v3FarTopRight, math::CVector3::Right);
    //pEngine->DrawLine(m_v3Pos + v3NearBottomLeft, m_v3Pos + v3FarBottomLeft, math::CVector3::Right);
    //pEngine->DrawLine(m_v3Pos + v3NearBottomRight, m_v3Pos + v3FarBottomRight, math::CVector3::Right);
  }
}
