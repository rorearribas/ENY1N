#pragma once
#include <windows.h>
#include <windowsx.h>

namespace render
{
  class CRenderWindow 
  {
  public:
    CRenderWindow() {}
    ~CRenderWindow() {}

    void CreateRenderWindow(const UINT32& _uWidth, const UINT32& _uHeight);
    void SetEnabled(bool _bEnabled) const;

    const HWND& GetHwnd() { return m_hWnd; }
    void SetHwnd(const HWND& _hWnd) { m_hWnd = _hWnd; }

    const UINT32& GetWidth() const { return m_uWidth; }
    const UINT32& GetHeight() const { return m_uHeight; }

  private:
    HWND m_hWnd = NULL;

    UINT32 m_uWidth = 0;
    UINT32 m_uHeight = 0;
  };
}



