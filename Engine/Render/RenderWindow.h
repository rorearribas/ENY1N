#pragma once
#include <windows.h>
#include <windowsx.h>

namespace render
{
  class CRenderWindow 
  {
  public:
    CRenderWindow(const UINT32& _uWidth, const UINT32& _uHeight);
    ~CRenderWindow() {}

    void SetEnabled(bool _bEnabled) const;
    void SetHwnd(const HWND& _hWnd) { m_hWnd = _hWnd; }

    const HWND& GetHwnd() { return m_hWnd; }
    inline const UINT32& GetWidth() const { return m_uRenderWindowX; }
    inline const UINT32& GetHeight() const { return m_uRenderWindowY; }

  private:
    HWND m_hWnd = nullptr;
    UINT32 m_uRenderWindowX = 0;
    UINT32 m_uRenderWindowY = 0;
  };
}



