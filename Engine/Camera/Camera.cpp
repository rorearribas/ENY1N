#include "Camera.h"
#include <windows.h>

namespace render
{
  CCamera::CCamera()
  {
    this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->posVector = XMLoadFloat3(&this->pos);
    this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateViewMatrix();
  }

  void CCamera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
  {
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
    this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
  }

  void CCamera::Update()
  {
    const float fCameraSpeed = 0.02f;

    if (GetAsyncKeyState('W') & 0x8000) AdjustPosition(GetForwardVector() * fCameraSpeed);
    if (GetAsyncKeyState('S') & 0x8000) AdjustPosition(GetBackwardVector() * fCameraSpeed);
    if (GetAsyncKeyState('D') & 0x8000) AdjustPosition(GetRightVector() * fCameraSpeed);
    if (GetAsyncKeyState('A') & 0x8000) AdjustPosition(GetLeftVector() * fCameraSpeed);
  }

  const XMMATRIX& CCamera::GetViewMatrix() const
  {
    return this->viewMatrix;
  }

  const XMMATRIX& CCamera::GetProjectionMatrix() const
  {
    return this->projectionMatrix;
  }

  const XMVECTOR& CCamera::GetPositionVector() const
  {
    return this->posVector;
  }

  const XMFLOAT3& CCamera::GetPositionFloat3() const
  {
    return this->pos;
  }

  const XMVECTOR& CCamera::GetRotationVector() const
  {
    return this->rotVector;
  }

  const XMFLOAT3& CCamera::GetRotationFloat3() const
  {
    return this->rot;
  }

  void CCamera::SetPosition(const XMVECTOR& pos)
  {
    XMStoreFloat3(&this->pos, pos);
    this->posVector = pos;
    this->UpdateViewMatrix();
  }

  void CCamera::SetPosition(float x, float y, float z)
  {
    this->pos = XMFLOAT3(x, y, z);
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateViewMatrix();
  }

  void CCamera::AdjustPosition(const XMVECTOR& pos)
  {
    this->posVector += pos;
    XMStoreFloat3(&this->pos, this->posVector);
    this->UpdateViewMatrix();
  }

  void CCamera::AdjustPosition(float x, float y, float z)
  {
    this->pos.x += x;
    this->pos.y += y;
    this->pos.z += z;
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateViewMatrix();
  }

  void CCamera::SetRotation(const XMVECTOR& rot)
  {
    this->rotVector = rot;
    XMStoreFloat3(&this->rot, rot);
    this->UpdateViewMatrix();
  }

  void CCamera::SetRotation(float x, float y, float z)
  {
    this->rot = XMFLOAT3(x, y, z);
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateViewMatrix();
  }

  void CCamera::AdjustRotation(const XMVECTOR& rot)
  {
    this->rotVector += rot;
    XMStoreFloat3(&this->rot, this->rotVector);
    this->UpdateViewMatrix();
  }

  void CCamera::AdjustRotation(float x, float y, float z)
  {
    this->rot.x += x;
    this->rot.y += y;
    this->rot.z += z;
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateViewMatrix();
  }

  void CCamera::SetLookAtPos(XMFLOAT3 _v3LookAtPos)
  {
    if (_v3LookAtPos.x == this->pos.x && _v3LookAtPos.y == this->pos.y && _v3LookAtPos.z == this->pos.z)
      return;

    _v3LookAtPos.x = this->pos.x - _v3LookAtPos.x;
    _v3LookAtPos.y = this->pos.y - _v3LookAtPos.y;
    _v3LookAtPos.z = this->pos.z - _v3LookAtPos.z;

    float fPitch = 0.0f;
    if (_v3LookAtPos.y != 0.0f)
    {
      const float fDistance = static_cast<float>(sqrt(_v3LookAtPos.x * _v3LookAtPos.x + _v3LookAtPos.z * _v3LookAtPos.z));
      fPitch = static_cast<float>(atan(_v3LookAtPos.y / fDistance));
    }

    float fYaw = 0.0f;
    if (_v3LookAtPos.x != 0.0f)
    {
      fYaw = static_cast<float>(atan(_v3LookAtPos.x / _v3LookAtPos.z));
    }
    if (_v3LookAtPos.z > 0)
      fYaw += XM_PI;

    SetRotation(fPitch, fYaw, 0.0f);
  }

  const XMVECTOR& CCamera::GetForwardVector()
  {
    return this->vec_forward;
  }

  const XMVECTOR& CCamera::GetRightVector()
  {
    return this->vec_right;
  }

  const XMVECTOR& CCamera::GetBackwardVector()
  {
    return this->vec_backward;
  }

  const XMVECTOR& CCamera::GetLeftVector()
  {
    return this->vec_left;
  }

  void CCamera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
  {
    //Calculate camera rotation matrix
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
    //Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
    XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
    //Adjust cam target to be offset by the camera's current position
    camTarget += this->posVector;
    //Calculate up direction based on current rotation
    XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
    //Rebuild view matrix
    this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);
    this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
  }
  

}