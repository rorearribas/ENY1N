#pragma once
#include "Libs/Utils/Singleton.h"
#include <windows.h>
#include "Libs/Maths/Vector2.h"
#include <unordered_map>

namespace input
{
  class CMouse {
  public:
    CMouse();
    ~CMouse();

    const maths::CVector2 GetMouseDelta() const;
    const maths::CVector2 GetMousePosition() const;
    const float GetMouseWheelDelta() const;

    const bool IsRightButtonPressed();
    const bool IsLeftButtonPressed();

  private:
    void OnUpdateMouse(RAWMOUSE*);
    friend class CInputManager;

    maths::CVector2 m_vMouseDelta = maths::CVector2::vEMPTY;
    maths::CVector2 m_vMousePosition = maths::CVector2::vEMPTY;
    float m_fMouseWheelDelta = 0.0f;
  };

  class CInputManager : public utils::CSingleton<CInputManager> 
  {
  public:
    CInputManager();
    ~CInputManager();

    void Flush();
    bool IsKeyPressed(USHORT _uKey) const;
    CMouse* GetMouse() const { return m_pMouse; }

  private:
    void OnUpdateKeyboard(RAWKEYBOARD*);

    std::unordered_map<USHORT, bool> m_mapKeyStates;
    CMouse* m_pMouse = nullptr;
  };
}


