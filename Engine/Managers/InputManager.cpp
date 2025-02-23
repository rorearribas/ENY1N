#include "InputManager.h"
#include "Engine/Global/GlobalResources.h"
#include <iostream>
#include <cassert>
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Macros/GlobalMacros.h"

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
    bool bOk = RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice));
    UNUSED_VARIABLE(bOk);
    assert(bOk);

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
    return maths::CVector2::Zero;
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
      const float fSmoothDelta = 0.05f;
      m_vMouseDelta.X = static_cast<float>(_pRawMouse->lLastX) * fSmoothDelta;
      m_vMouseDelta.Y = static_cast<float>(_pRawMouse->lLastY) * fSmoothDelta;
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

    // Register keyboard input
    bool bOk = RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice));
    UNUSED_VARIABLE(bOk);
    assert(bOk);

    // Register all keys
    RegisterKeys();

    // Bind delegate
    global::delegates::s_oOnUpdateKeyboardDelegate.Bind(&CInputManager::OnUpdateKeyboard, this);
  }
  void CInputManager::RegisterKeys()
  {
    // Register all keys
    for (USHORT uKey = VK_LBUTTON; uKey <= VK_OEM_CLEAR; ++uKey) 
    {
      RAWKEYBOARD oRawKeyboard = {};
      oRawKeyboard.VKey = uKey;
      oRawKeyboard.Flags = 0;
      m_mapKeyStates[oRawKeyboard.VKey] = false;
    }
  }
  // ------------------------------------
  CInputManager::~CInputManager()
  {
    if (m_pMouse)
    {
      delete m_pMouse;
      m_pMouse = nullptr;
    }
    m_mapKeyStates.clear();
  }
  // ------------------------------------
  void CInputManager::Flush()
  {
    if (m_pMouse)
    {
      m_pMouse->m_vMouseDelta = maths::CVector2::Zero;
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
    if (_pRawKeyboard && !m_mapKeyStates.empty())
    {
      // Set value
      auto it = m_mapKeyStates.find(_pRawKeyboard->VKey);
      it->second = !(_pRawKeyboard->Flags & RI_KEY_BREAK);
    }
  }
}