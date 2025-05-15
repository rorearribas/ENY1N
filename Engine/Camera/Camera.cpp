#include "Camera.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Base/Engine.h"
#include "Engine/Managers/InputManager.h"
#include "Libs/Math/Math.h"
#include <ostream>
#include <iostream>

namespace render
{
  namespace internal_camera
  {
    const float s_fInterpolateSpeed = 10.0f;

    static bool s_bWasRightButtonPressed = false;
    static math::CVector2 s_v2LastPosition = math::CVector2::Zero;
  }
  // ------------------------------------
  CCamera::CCamera()
  {
    m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
  }
  // ------------------------------------
  void CCamera::Update(float _fDeltaTime)
  {
    // Keyboard movement
    math::CMatrix4x4 mRotMatrix = math::CMatrix4x4::Rotation(m_vRot);
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
    m_fTargetFov = math::lerp(m_fTargetFov, m_fFov, internal_camera::s_fInterpolateSpeed * _fDeltaTime);

    // Update perspective matrix
    UpdatePerspectiveMatrix();

    // Update constant buffer
    m_oConstantBuffer.GetData().mMatrix = math::CMatrix4x4::Transpose(m_mViewMatrix * m_mProjectionMatrix);
    m_oConstantBuffer.UpdateBuffer();

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
    m_vPos = newPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::MovePosition(const math::CVector3& vDelta) 
  {
    m_vPos += vDelta;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(const math::CVector3& _vNewRot) 
  {
    m_vRot = _vNewRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AddRotation(const math::CVector3& _vDeltaRot) 
  {
    m_vRot += _vDeltaRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetLookAtPos(const math::CVector3& _v3LookAt) 
  {
    if (_v3LookAt == m_vPos)
      return;

    math::CVector3 v3Dir = _v3LookAt - m_vPos;
    float fPitch = (float)atan2(v3Dir.Y, sqrt(v3Dir.X * v3Dir.X + v3Dir.Z * v3Dir.Z));
    float fYaw = (float)atan2(v3Dir.X, v3Dir.Z);
    if (v3Dir.Z > 0) fYaw += static_cast<float>(math::s_fPI);

    SetRotation(math::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::UpdatePerspectiveMatrix()
  {
    float fRadians =  math::DegreesToRadians(m_fTargetFov);
    m_mProjectionMatrix = math::CMatrix4x4::CreatePerspectiveMatrix(fRadians, m_fAspectRatio, m_fNear, m_fFar);
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix() 
  {
    // Clamp pitch value
    m_vRot.X = math::Clamp(m_vRot.X, -90.0f, 90.0f);

    // Create the rotation matrix
    math::CMatrix4x4 mRotationMatrix = math::CMatrix4x4::Rotation(m_vRot);

    // Calculate target offset
    math::CVector3 vTarget = mRotationMatrix * math::CVector3::Forward;
    vTarget = vTarget + m_vPos;

    // Calculate up direction
    math::CVector3 vUpDir = mRotationMatrix * math::CVector3::Up;

    // Set view matrix
    m_mViewMatrix = math::CMatrix4x4::LookAt(m_vPos, vTarget, vUpDir);
  }
}
