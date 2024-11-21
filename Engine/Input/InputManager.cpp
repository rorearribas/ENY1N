#include "InputManager.h"
#include "Engine/Global/GlobalResources.h"
#include <iostream>
#include <cassert>
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Maths/Maths.h"

namespace input
{
  CMouse::CMouse()
  {
    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = {};
    oRawInputDevice.usUsagePage = 0x01;
    oRawInputDevice.usUsage = 0x02;
    oRawInputDevice.dwFlags = RIDEV_INPUTSINK;
    oRawInputDevice.hwndTarget = global::window::s_oHwnd;

    // Register raw input
    assert(RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice)));

    // Bind delegate
    global::delegates::s_oUpdateMouseDelegate.Bind(&CMouse::OnUpdateMouse, this);
  }
  // ------------------------------------
  CMouse::~CMouse()
  {
    global::delegates::s_oUpdateMouseDelegate.Clear();
  }
  // ------------------------------------
  const maths::CVector2 CMouse::GetMouseDelta() const
  {
    return m_vMouseDelta;
  }
  // ------------------------------------
  const maths::CVector2 CMouse::GetMousePosition() const
  {
    POINT oScreenPoint = {};
    if (ScreenToClient(global::window::s_oHwnd, &oScreenPoint))
    {
      return maths::CVector2((float)oScreenPoint.x, (float)oScreenPoint.y);
    }
    return maths::CVector2::vEMPTY;
  }
  // ------------------------------------
  const float CMouse::GetMouseWheelDelta() const
  {
    return m_fMouseWheelDelta;
  }
  // ------------------------------------
  const bool CMouse::IsRightButtonPressed()
  {
    return GetAsyncKeyState(VK_RBUTTON);
  }
  // ------------------------------------
  const bool CMouse::IsLeftButtonPressed()
  {
    return GetAsyncKeyState(VK_LBUTTON);
  }
  // ------------------------------------
  void CMouse::OnUpdateMouse(RAWMOUSE* _pRawMouse)
  {
    // Set mouse delta
    if (_pRawMouse && _pRawMouse->usButtonFlags == MOUSE_MOVE_RELATIVE)
    {
      // Get values
      float fDeltaX = static_cast<float>(_pRawMouse->lLastX);
      float fDeltaY = static_cast<float>(_pRawMouse->lLastY);
      // Interpolation
      m_vMouseDelta.X = maths::Lerp(m_vMouseDelta.X, fDeltaX, 0.01f);
      m_vMouseDelta.Y = maths::Lerp(m_vMouseDelta.Y, fDeltaY, 0.01f);
    }

    // Set mouse wheel delta
    if (_pRawMouse && _pRawMouse->usButtonFlags & RI_MOUSE_WHEEL)
    {
      m_fMouseWheelDelta = static_cast<float>((*(short*)&_pRawMouse->usButtonData) / WHEEL_DELTA);
    }
  }
  // ------------------------------------
  CInputManager::CInputManager()
  {
    // Create mouse instance
    m_pMouse = new CMouse();

    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = {};
    oRawInputDevice.usUsagePage = 0x01;
    oRawInputDevice.usUsage = 0x06;
    oRawInputDevice.dwFlags = RIDEV_INPUTSINK;
    oRawInputDevice.hwndTarget = global::window::s_oHwnd;

    // Register raw input
    assert(RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice)));

    // Bind delegate
    global::delegates::s_oUpdateKeyboardDelegate.Bind(&CInputManager::OnUpdateKeyboard, this);
  }
  // ------------------------------------
  CInputManager::~CInputManager()
  {
    if (m_pMouse)
    {
      delete m_pMouse;
      m_pMouse = nullptr;
    }
  }
  // ------------------------------------
  void CInputManager::Flush()
  {
    if (m_pMouse)
    {
      m_pMouse->m_vMouseDelta = maths::CVector2::vEMPTY;
      m_pMouse->m_vMousePosition = maths::CVector2::vEMPTY;
      m_pMouse->m_fMouseWheelDelta = 0.0f;
    }
  }
  // ------------------------------------
  bool CInputManager::IsKeyPressed(USHORT _uKey) const
  {
    return m_mapKeyStates.count(_uKey) ? m_mapKeyStates.at(_uKey) : false;
  }
  // ------------------------------------
  void CInputManager::OnUpdateKeyboard(RAWKEYBOARD* _pRawKeyboard)
  {
    if (_pRawKeyboard)
    {
      USHORT uKey = _pRawKeyboard->VKey;
      bool bIsKeyDown = !(_pRawKeyboard->Flags & RI_KEY_BREAK);

      // Register key or set to pressed
      auto [it, inserted] = m_mapKeyStates.emplace(uKey, bIsKeyDown);
      if (!inserted) { it->second = bIsKeyDown; }
    }
  }
}