#include "Camera.h"
#include "Engine/Global/GlobalResources.h"
#include "Libs/ImGui/imgui.h"
#include "Engine/Base/Engine.h"
#include "Engine/Input/InputManager.h"
#include <algorithm>

#include <iostream>
namespace render
{
  namespace internal_camera
  {
    const float s_fPI(3.14159274f);
  }

  CCamera::CCamera()
  {
    m_oConstantBuffer.Initialize(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
  }
  // ------------------------------------
  void CCamera::Update(float _fDeltaTime)
  {
    // Input
    input::CInputManager* pInputManager = input::CInputManager::GetInstance();
    input::CMouse* pMouse = pInputManager->GetMouse();

    // Movimiento de teclado
    maths::CMatrix4x4 mRotMatrix = maths::CMatrix4x4::Rotation(m_vRot);
    maths::CVector3 vForward = (mRotMatrix * maths::CVector3::Forward).Normalized();
    maths::CVector3 vRight = (mRotMatrix * maths::CVector3::Right).Normalized();

    if (pInputManager->IsKeyPressed('W')) MovePosition(vForward * m_fMovementSpeed * _fDeltaTime);
    if (pInputManager->IsKeyPressed('S')) MovePosition(-vForward * m_fMovementSpeed * _fDeltaTime);
    if (pInputManager->IsKeyPressed('D')) MovePosition(vRight * m_fMovementSpeed * _fDeltaTime);
    if (pInputManager->IsKeyPressed('A')) MovePosition(-vRight * m_fMovementSpeed * _fDeltaTime);

    // Movimiento del ratón
    bool bMousePressed = pMouse->IsRightButtonPressed();
    ShowCursor(bMousePressed);

    float xValue = pMouse->GetMouseDelta().X * m_fCameraSpeed * _fDeltaTime;
    float yValue = pMouse->GetMouseDelta().Y * m_fCameraSpeed * _fDeltaTime;
    AddRotation(bMousePressed ? maths::CVector3(yValue, xValue, 0.0f) : maths::CVector3::Zero);

    // Actualización de matrices
    UpdatePerspectiveMatrix(); // Asegúrate de que actualiza solo la proyección
    maths::CMatrix4x4 mViewProjection = m_mViewMatrix * m_mProjectionMatrix;

    // Actualización del buffer constante
    m_oConstantBuffer.GetData().mMatrix = maths::CMatrix4x4::Transpose(mViewProjection);
    assert(m_oConstantBuffer.UpdateBuffer() && "Failed to update constant buffer.");

    ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
    global::dx11::s_pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
  }
  // ------------------------------------
  void CCamera::ShowCursor(bool _bMousePressed)
  {
    if (_bMousePressed)
    {
      // Get rect
      RECT oRect = {};
      GetClientRect(global::window::s_oHwnd, &oRect);

      // Calculate center
      POINT oCenter = {};
      oCenter.x = (oRect.left + oRect.right) / 2;
      oCenter.y = (oRect.top + oRect.bottom) / 2;

      // Set cursor in the middle of the screen
      ClientToScreen(global::window::s_oHwnd, &oCenter);
      SetCursorPos(oCenter.x, oCenter.y);
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

    if (v3Dir.Z > 0)
      fYaw += internal_camera::s_fPI;

    SetRotation(maths::CVector3(fPitch, fYaw, 0.0f));
  }
  // ------------------------------------
  void CCamera::UpdatePerspectiveMatrix()
  {
    float fRadians = m_fFov * (internal_camera::s_fPI / 180.0f);
    m_mProjectionMatrix = maths::CMatrix4x4::CreatePerspectiveMatrix(fRadians, m_fAspectRatio, m_fNear, m_fFar);
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix() 
  {
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
