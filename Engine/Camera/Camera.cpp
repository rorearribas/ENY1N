#include "Camera.h"
#include "Engine/Global/DX11GlobalInterface.h"
#include "Libs/ImGui/imgui.h"
#include "../Base/Engine.h"

namespace render
{
  namespace internal_camera
  {
    const float s_fPI(3.14159274f);
  }

  CCamera::CCamera()
  {
    m_oConstantBuffer.Initialize(global::dx11::s_pDX11Device, global::dx11::s_pDX11DeviceContext);
  }
  // ------------------------------------
  void CCamera::Update()
  {
    const float fCameraSpeed = 0.02f;

    maths::CMatrix4x4 mRotMatrix = maths::CMatrix4x4::Rotation(0.0f, m_vRot.Y, 0.0f);
    if (GetAsyncKeyState('W') & 0x8000) AdjustPosition(mRotMatrix * maths::CVector3::Forward * fCameraSpeed);
    if (GetAsyncKeyState('S') & 0x8000) AdjustPosition(mRotMatrix * (maths::CVector3::Forward * -1.0f) * fCameraSpeed);
    if (GetAsyncKeyState('D') & 0x8000) AdjustPosition(mRotMatrix * maths::CVector3::Right * fCameraSpeed);
    if (GetAsyncKeyState('A') & 0x8000) AdjustPosition(mRotMatrix * (maths::CVector3::Right * -1.0f) * fCameraSpeed);
    if (GetAsyncKeyState('Q') & 0x8000) AdjustPosition(mRotMatrix * (maths::CVector3::Up * 1.0f) * fCameraSpeed);
    if (GetAsyncKeyState('E') & 0x8000) AdjustPosition(mRotMatrix * (maths::CVector3::Up * -1.0f) * fCameraSpeed);

    // Rotation
    ImGuiIO& iO = ImGui::GetIO();
    ImVec2 v2MouseDelta = iO.MouseDelta;
    maths::CVector3 v3Delta(v2MouseDelta.y * 0.001f, v2MouseDelta.x * 0.001f, 0.0f);

    bool bMousePressed = iO.MouseDown[1];
    AdjustRotation(bMousePressed ? v3Delta : maths::CVector3::Zero);

    if (bMousePressed) { while (ShowCursor(FALSE) >= 0); }
    else { while (ShowCursor(TRUE) < 0); }

    // Update perspective matrix
    UpdatePerspectiveMatrix();

    // Calculate MVP matrix
    maths::CMatrix4x4 mViewProjection = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 mMatrix = mViewProjection * m_mViewMatrix * m_mProjectionMatrix;
    m_oConstantBuffer.GetCurrentData().mMatrix = maths::CMatrix4x4::Transpose(mMatrix);

    if (!m_oConstantBuffer.Apply()) return;

    ID3D11Buffer* pBuffer = m_oConstantBuffer.GetBuffer();
    global::dx11::s_pDX11DeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
  }
  // ------------------------------------
  void CCamera::SetPosition(const maths::CVector3& newPos) 
  {
    m_vPos = newPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetPosition(float _x, float _y, float _z)
  {
    m_vPos = maths::CVector3(_x, _y, _z);
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustPosition(const maths::CVector3& vDelta) 
  {
    m_vPos += vDelta;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustPosition(float x, float y, float z) 
  {
    m_vPos.X += x;
    m_vPos.Y += y;
    m_vPos.Z += z;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(const maths::CVector3& _vNewRot) 
  {
    m_vRot = _vNewRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(float x, float y, float z) 
  {
    m_vRot = maths::CVector3(x, y, z);
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustRotation(const maths::CVector3& _vDeltaRot) 
  {
    m_vRot += _vDeltaRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustRotation(float x, float y, float z) 
  {
    m_vRot.X += x;
    m_vRot.Y += y;
    m_vRot.Z += z;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetLookAtPos(const maths::CVector3& targetPos) 
  {
    if (targetPos == m_vPos)
      return;

    maths::CVector3 direction = targetPos - m_vPos;
    float fPitch = (float)atan2(direction.Y, sqrt(direction.X * direction.X + direction.Z * direction.Z));
    float fYaw = (float)atan2(direction.X, direction.Z);

    if (direction.Z > 0)
      fYaw += internal_camera::s_fPI;

    SetRotation(fPitch, fYaw, 0.0f);
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
    maths::CMatrix4x4 mRotationMatrix = maths::CMatrix4x4::Rotation(m_vRot.X, m_vRot.Y, m_vRot.Z);

    // Calculate target offset
    maths::CVector3 vTarget = mRotationMatrix * maths::CVector3::Forward;
    vTarget = vTarget + m_vPos;

    // Calculate up direction
    maths::CVector3 vUpDir = mRotationMatrix * maths::CVector3::Up;

    // Create look at
    m_mViewMatrix = maths::CMatrix4x4::LookAt(m_vPos, vTarget, vUpDir);
  }
}
