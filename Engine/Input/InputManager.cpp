#include "InputManager.h"
#include "Engine/Global/GlobalResources.h"
#include <iostream>
#include <cassert>
#include "Engine/Base/Engine.h"
#include "Engine/Render/Render.h"
#include "Libs/ImGui/imgui.h"

namespace input
{
  CMouse::CMouse()
  {
    // Bind delegate
    global::delegates::s_oUpdateMouseDelegate.Bind(&CMouse::OnUpdateMouse, this);

    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = {};
    oRawInputDevice.usUsagePage = 0x01;
    oRawInputDevice.usUsage = 0x02;
    oRawInputDevice.dwFlags = RIDEV_INPUTSINK;
    oRawInputDevice.hwndTarget = global::window::s_oHwnd;

    // Register raw input
    assert(RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice)));
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
    return ImGui::GetIO().MouseWheel;
  }
  // ------------------------------------
  const bool CMouse::IsRightButtonPressed()
  {
    return ImGui::GetIO().MouseDown[1];
  }
  // ------------------------------------
  const bool CMouse::IsLeftButtonPressed()
  {
    return ImGui::GetIO().MouseDown[0];
  }
  // ------------------------------------
  void CMouse::OnUpdateMouse(RAWMOUSE* _pRawMouse)
  {
    // Set mouse delta
    if (_pRawMouse && _pRawMouse->usButtonFlags == MOUSE_MOVE_RELATIVE)
    {
      m_vMouseDelta.X = static_cast<float>(_pRawMouse->lLastX);
      m_vMouseDelta.Y = static_cast<float>(_pRawMouse->lLastY);
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
    // Bind delegate
    global::delegates::s_oUpdateKeyboardDelegate.Bind(&CInputManager::OnUpdateKeyboard, this);

    // Raw input config
    RAWINPUTDEVICE oRawInputDevice = {};
    oRawInputDevice.usUsagePage = 0x01;
    oRawInputDevice.usUsage = 0x06;
    oRawInputDevice.dwFlags = RIDEV_INPUTSINK;
    oRawInputDevice.hwndTarget = global::window::s_oHwnd;

    // Register raw input
    assert(RegisterRawInputDevices(&oRawInputDevice, 1, sizeof(oRawInputDevice)));

    // Create mouse instance
    m_pMouse = new CMouse();
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
  std::string GetKeyNameFromVirtualKey(USHORT virtualKey)
  {
    // Obtener el nombre de la tecla desde el virtual key code
    char keyName[128] = {};
    if (GetKeyNameTextA(MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) << 16, keyName, sizeof(keyName)) > 0)
    {
      return std::string(keyName);
    }
    return "Unknown Key";
  }
  // ------------------------------------
  void CInputManager::OnUpdateKeyboard(RAWKEYBOARD* _pRawKeyboard)
  {
    if (_pRawKeyboard)
    {
      USHORT virtualKey = _pRawKeyboard->VKey;
      bool isKeyDown = !(_pRawKeyboard->Flags & RI_KEY_BREAK);

      auto [it, inserted] = m_mapKeyStates.emplace(virtualKey, isKeyDown);
      if (!inserted) { it->second = isKeyDown; }
    }
  }
}