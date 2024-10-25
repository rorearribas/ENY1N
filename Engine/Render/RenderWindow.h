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
    void ShowRenderWindow();
    void UpdateWindow();

    const HWND& GetHwnd() { return m_hWnd; }
    void SetHwnd(const HWND& _hWnd) { m_hWnd = _hWnd; }

  private:
    HWND m_hWnd = NULL;
  };
}



