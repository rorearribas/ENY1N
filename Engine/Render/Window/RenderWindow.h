#pragma once
#include <windows.h>
#include <stdint.h>

namespace render
{
  class CRenderWindow
  {
  public:
    CRenderWindow(uint32_t _uWidth, uint32_t _uHeight);
    ~CRenderWindow() {}

    void SetEnabled(bool _bEnabled) const;
    inline const HWND& GetHwnd() const { return m_hWnd; }

    const uint32_t GetWidth() const;
    const uint32_t GetHeight() const;

  private:
    HWND m_hWnd = nullptr;
  };
}



