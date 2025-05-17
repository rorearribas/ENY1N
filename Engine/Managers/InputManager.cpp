#include "InputManager.h"
#include "Engine/Global/GlobalResources.h"
#include <iostream>
#include <cassert>
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Libs/ImGui/imgui.h"
#include "Libs/Macros/GlobalMacros.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif
#ifndef HID_USAGE_GENERIC_KEYBOARD
#define HID_USAGE_GENERIC_KEYBOARD        ((USHORT) 0x06)
#endif

namespace input
{
  // ------------------------------------
  // --------------MOUSE-----------------
  // ------------------------------------
  CMouse::CMouse()
  {
    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = RAWINPUTDEVICE();
    oRawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
    oRawInputDevice.usUsage = HID_USAGE_GENERIC_MOUSE;
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
  const math::CVector2 CMouse::GetMouseDelta() const
  {
    return m_vMouseDelta;
  }
  // ------------------------------------
  const math::CVector2 CMouse::GetMousePosition() const
  {
    POINT oScreenPoint = POINT();
    return GetCursorPos(&oScreenPoint) ?
    math::CVector2(static_cast<float>(oScreenPoint.x), static_cast<float>(oScreenPoint.y)) : math::CVector2::Zero;
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
      const float fPollingRate = 1.0f / 1000.0f;
      m_vMouseDelta.X += static_cast<float>(_pRawMouse->lLastX) * fPollingRate;
      m_vMouseDelta.Y += static_cast<float>(_pRawMouse->lLastY) * fPollingRate;
    }

    // Set mouse wheel delta
    if (_pRawMouse && _pRawMouse->usButtonFlags & RI_MOUSE_WHEEL)
    {
      m_fMouseWheelDelta = static_cast<float>((*(short*)&_pRawMouse->usButtonData) / WHEEL_DELTA);
    }
  }
  // ------------------------------------
  // ------------KEYBOARD----------------
  // ------------------------------------
  CKeyboard::CKeyboard()
  {
    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = RAWINPUTDEVICE();
    oRawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
    oRawInputDevice.usUsage = HID_USAGE_GENERIC_KEYBOARD;
    oRawInputDevice.dwFlags = RIDEV_INPUTSINK;
    oRawInputDevice.hwndTarget = global::window::s_oHwnd;

    // Register keyboard input
    bool bOk = RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice));
    UNUSED_VARIABLE(bOk);
    assert(bOk);

    // Register all keys
    RegisterKeys();

    // Bind delegate
    global::delegates::s_oOnUpdateKeyboardDelegate.Bind(&CKeyboard::OnUpdateKeyboard, this);
  }
  // ------------------------------------
  CKeyboard::~CKeyboard()
  {
    m_mapKeyStates.clear();
    global::delegates::s_oOnUpdateKeyboardDelegate.Clear();
  }
  // ------------------------------------
  bool CKeyboard::IsKeyPressed(USHORT _uKey) const
  {
    return m_mapKeyStates.count(_uKey) ? m_mapKeyStates.at(_uKey) : false;
  }
  // ------------------------------------
  void CKeyboard::OnUpdateKeyboard(RAWKEYBOARD* _pPtr)
  {
    if (_pPtr && !m_mapKeyStates.empty())
    {
      // Set value
      auto it = m_mapKeyStates.find(_pPtr->VKey);
      it->second = !(_pPtr->Flags & RI_KEY_BREAK);
    }
  }
  // ------------------------------------
  void CKeyboard::RegisterKeys()
  {
    // Register all keys
    for (USHORT uKey = VK_LBUTTON; uKey <= VK_OEM_CLEAR; ++uKey)
    {
      RAWKEYBOARD oRawKeyboard = RAWKEYBOARD();
      oRawKeyboard.VKey = uKey;
      oRawKeyboard.Flags = 0;
      m_mapKeyStates[oRawKeyboard.VKey] = false;
    }
  }
  // ------------------------------------
  // -----------INPUT_MANAGER------------
  // ------------------------------------
  CInputManager::CInputManager()
  {
    // Create mouse instance
    m_pMouse = new CMouse();
    // Create keyboard instance
    m_pKeyboard = new CKeyboard();
  }
  // ------------------------------------
  CInputManager::~CInputManager()
  {
    Clean();
  }
  // ------------------------------------
  void CInputManager::Flush()
  {
    if (m_pMouse)
    {
      m_pMouse->m_vMouseDelta = math::CVector2::Zero;
      m_pMouse->m_fMouseWheelDelta = 0.0f;
    }
  }
  // ------------------------------------
  void CInputManager::Clean()
  {
    global::ReleaseObject(m_pMouse);
    global::ReleaseObject(m_pKeyboard);
  }
  // ------------------------------------
  bool CInputManager::IsKeyPressed(USHORT _uKey) const
  {
    return m_pKeyboard ? m_pKeyboard->IsKeyPressed(_uKey) : false;
  }
}