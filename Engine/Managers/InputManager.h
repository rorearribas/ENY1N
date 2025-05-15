#pragma once
#include "Libs/Utils/Singleton.h"
#include <windows.h>
#include "Libs/Math/Vector2.h"
#include <map>

namespace input
{
  class CMouse {
  public:
    CMouse();
    ~CMouse();

    const math::CVector2 GetMousePosition() const;
    const math::CVector2 GetMouseDelta() const;
    const float GetMouseWheelDelta() const;

    const bool IsRightButtonPressed();
    const bool IsLeftButtonPressed();

  private:
    void OnUpdateMouse(RAWMOUSE*);
    friend class CInputManager;

    math::CVector2 m_vMouseDelta = math::CVector2::Zero;
    float m_fMouseWheelDelta = 0.0f;
  };

  class CKeyboard
  {
  public:
    CKeyboard();
    ~CKeyboard();

    bool IsKeyPressed(USHORT _uKey) const;

  protected:
    void OnUpdateKeyboard(RAWKEYBOARD*);
  private:
    void RegisterKeys();
    std::map<USHORT, bool> m_mapKeyStates;
  };

  class CInputManager : public utils::CSingleton<CInputManager> 
  {
  public:
    CInputManager();
    ~CInputManager();

    CMouse* GetMouse() const { return m_pMouse; }
    CKeyboard* GetKeyboard() const { return m_pKeyboard; }

    void Flush();
    bool IsKeyPressed(USHORT _uKey) const;

  private:
    void Clean();

    CMouse* m_pMouse = nullptr;
    CKeyboard* m_pKeyboard = nullptr;
  };
}


