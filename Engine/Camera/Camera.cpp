#include "Camera.h"
#include <DirectXMath.h>
#include "Engine/Global/DX11GlobalInterface.h"
#include <windows.h>

namespace render
{
  CCamera::CCamera() : m_vPos(maths::CVector3(0.0f, 0.0f, 0.0f)),
    m_vRot(maths::CVector3(0.0f, 0.0f, 0.0f)), posVector(m_vPos), rotVector(m_vRot)
  {
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ) 
  {
    float fovRadians = fovDegrees * (DirectX::XM_PI / 180.0f);
    this->m_mProjectionMatrix = maths::CMatrix4x4::CreatePerspectiveMatrix(fovRadians, aspectRatio, nearZ, farZ);
  }
  // ------------------------------------
  void CCamera::SetupCamera(const SCameraSettings& _oCameraSettings)
  {
    m_oConstantBuffer.Initialize(global::dx11::s_pDX11Device, global::dx11::s_pDX11DeviceContext);
    SetProjectionValues(_oCameraSettings.m_fFov, _oCameraSettings.m_fAspectRatio, _oCameraSettings.m_fNear, _oCameraSettings.m_fFar);
  }
  // ------------------------------------
  void CCamera::Update()
  {
    const float cameraSpeed = 0.02f;

    if (GetAsyncKeyState('W') & 0x8000) AdjustPosition(GetForwardVector() * cameraSpeed);
    if (GetAsyncKeyState('S') & 0x8000) AdjustPosition(GetBackwardVector() * cameraSpeed);
    if (GetAsyncKeyState('D') & 0x8000) AdjustPosition(GetRightVector() * cameraSpeed);
    if (GetAsyncKeyState('A') & 0x8000) AdjustPosition(GetLeftVector() * cameraSpeed);

    // Update matrix
    maths::CMatrix4x4 mViewProjection = maths::CMatrix4x4::Identity;
    maths::CMatrix4x4 mMatrix = mViewProjection * GetViewMatrix() * GetProjectionMatrix();
    m_oConstantBuffer.GetCurrentData().mMatrix = maths::CMatrix4x4::Transpose(mMatrix);

    if (!m_oConstantBuffer.Apply()) return;

    ID3D11Buffer* pBuffer = m_oConstantBuffer.GetBuffer();
    global::dx11::s_pDX11DeviceContext->VSSetConstantBuffers(0, 1, &pBuffer);
  }

  const maths::CMatrix4x4& CCamera::GetViewMatrix() const 
  {
    return this->m_mViewMatrix;
  }
  // ------------------------------------
  const maths::CMatrix4x4& CCamera::GetProjectionMatrix() const {
    return this->m_mProjectionMatrix;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetPositionVector() const {
    return this->posVector;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetPositionFloat3() const {
    return this->m_vPos;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetRotationVector() const {
    return this->rotVector;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetRotationFloat3() const {
    return this->m_vRot;
  }
  // ------------------------------------
  void CCamera::SetPosition(const maths::CVector3& newPos) {
    m_vPos = newPos;
    posVector = m_vPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetPosition(float x, float y, float z) {
    m_vPos = maths::CVector3(x, y, z);
    posVector = m_vPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustPosition(const maths::CVector3& deltaPos) {
    posVector += deltaPos;
    m_vPos = posVector;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustPosition(float x, float y, float z) {
    m_vPos.X += x;
    m_vPos.Y += y;
    m_vPos.Z += z;
    posVector = m_vPos;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(const maths::CVector3& newRot) {
    rotVector = newRot;
    m_vRot = rotVector;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::SetRotation(float x, float y, float z) {
    m_vRot = maths::CVector3(x, y, z);
    rotVector = m_vRot;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustRotation(const maths::CVector3& deltaRot) {
    rotVector += deltaRot;
    m_vRot = rotVector;
    UpdateViewMatrix();
  }
  // ------------------------------------
  void CCamera::AdjustRotation(float x, float y, float z) {
    m_vRot.X += x;
    m_vRot.Y += y;
    m_vRot.Z += z;
    rotVector = m_vRot;
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
      fYaw += DirectX::XM_PI;

    SetRotation(fPitch, fYaw, 0.0f);
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetForwardVector() {
    return this->vec_forward;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetRightVector() {
    return this->vec_right;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetBackwardVector() {
    return this->vec_backward;
  }
  // ------------------------------------
  const maths::CVector3& CCamera::GetLeftVector() {
    return this->vec_left;
  }
  // ------------------------------------
  void CCamera::UpdateViewMatrix() 
  {
    // Crear la matriz de rotación de la cámara
    maths::CMatrix4x4 camRotationMatrix = maths::CMatrix4x4::Rotation(m_vRot.X, m_vRot.Y, m_vRot.Z);

    // Calcular el vector de dirección hacia adelante de la cámara
    maths::CVector3 camTarget = camRotationMatrix * maths::CVector3::Forward;
    camTarget = camTarget + m_vPos;

    // Calcular el vector de dirección hacia arriba de la cámara
    maths::CVector3 upDir = camRotationMatrix * maths::CVector3::Up;

    // Crear la matriz de vista personalizada
    this->m_mViewMatrix = maths::CMatrix4x4::LookAt(m_vPos, camTarget, upDir);

    // Actualizar los vectores de movimiento con la nueva rotación
    maths::CMatrix4x4 vecRotationMatrix = maths::CMatrix4x4::Rotation(0.0f, m_vRot.Y, 0.0f);
    this->vec_forward = vecRotationMatrix * maths::CVector3::Forward;
    this->vec_backward = vecRotationMatrix * (maths::CVector3::Forward * -1.0f);
    this->vec_right = vecRotationMatrix * maths::CVector3::Right;
    this->vec_left = vecRotationMatrix * (maths::CVector3::Right * -1.0f);
  }
}
