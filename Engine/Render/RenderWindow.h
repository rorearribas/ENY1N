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
    const HWND& GetHwnd() const;
    const UINT32 GetWidth() const;
    const UINT32 GetHeight() const;

  private:
    HWND m_hWnd = nullptr;
  };
}



