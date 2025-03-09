#include "Camera.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Base/Engine.h"
#include "Engine/Managers/InputManager.h"
#include "Libs/Maths/Maths.h"
#include <ostream>
#include <iostream>

namespace render
{
  namespace internal_camera
  {
    const float s_fInterpolateSpeed = 10.0f;

    static bool s_bWasRightButtonPressed = false;
    static maths::CVector2 s_v2LastPosition = maths::CVector2::Zero;
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
    maths::CMatrix4x4 mRotMatrix = maths::CMatrix4x4::Rotation(m_vRot);
    maths::CVector3 vForward = (mRotMatrix * maths::CVector3::Forward).Normalized();
    maths::CVector3 vRight = (mRotMatrix * maths::CVector3::Right).Normalized();

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

      maths::CVector3 v3AddedRot(maths::RadiansToDegrees(yValue), maths::RadiansToDegrees(xValue), 0.0f);
      AddRotation(v3AddedRot);
    }

    // Wheel 
    if (!bRightButtonPressed)
    {
      float fMouseDelta = pMouse->GetMouseWheelDelta();
      MovePosition(fMouseDelta != 0 ? vForward * (fMouseDelta * m_fMovementSpeed) * _fDeltaTime : maths::CVector3::Zero);
    }

    // Interpolate FOV
    m_fTargetFov = maths::lerp(m_fTargetFov, m_fFov, internal_camera::s_fInterpolateSpeed * _fDeltaTime);

    // Update perspective matrix
    UpdatePerspectiveMatrix();

    // Update constant buffer
    m_oConstantBuffer.GetData().mMatrix = maths::CMatrix4x4::Transpose(m_mViewMatrix * m_mProjectionMatrix);
    m_oConstantBuffer.UpdateBuffer();

    ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
    global::dx11::s_pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
  }
  // ------------------------------------
  void CCamera::ShowCursor(bool _bMousePressed, const maths::CVector2& _vMousePos)
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
  void CCamera::SetPosition(const maths::CVector3& newPos) 
  {
    m_vPos = newPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::MovePosition(const maths::CVector3& vDelta) 
  {
    m_vPos += vDelta;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(const maths::CVector3& _vNewRot) 
  {
    m_vRot = _vNewRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AddRotation(const maths::CVector3& _vDeltaRot) 
  {
    m_vRot += _vDeltaRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetLookAtPos(const maths::CVector3& _v3LookAt) 
  {
    if (_v3LookAt == m_vPos)
      return;

    maths::CVector3 v3Dir = _v3LookAt - m_vPos;
    float fPitch = (float)atan2(v3Dir.Y, sqrt(v3Dir.X * v3Dir.X + v3Dir.Z * v3Dir.Z));
    float fYaw = (float)atan2(v3Dir.X, v3Dir.Z);
    if (v3Dir.Z > 0) fYaw += static_cast<float>(maths::s_fPI);

    SetRotation(maths::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::UpdatePerspectiveMatrix()
  {
    float fRadians =  maths::DegreesToRadians(m_fTargetFov);
    m_mProjectionMatrix = maths::CMatrix4x4::CreatePerspectiveMatrix(fRadians, m_fAspectRatio, m_fNear, m_fFar);
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix() 
  {
    // Clamp pitch value
    m_vRot.X = maths::clamp<float>(m_vRot.X, -90.0f, 90.0f);

    // Create the rotation matrix
    maths::CMatrix4x4 mRotationMatrix = maths::CMatrix4x4::Rotation(m_vRot);

    // Calculate target offset
    maths::CVector3 vTarget = mRotationMatrix * maths::CVector3::Forward;
    vTarget = vTarget + m_vPos;

    // Calculate up direction
    maths::CVector3 vUpDir = mRotationMatrix * maths::CVector3::Up;

    // Set view matrix
    m_mViewMatrix = maths::CMatrix4x4::LookAt(m_vPos, vTarget, vUpDir);
  }
}
