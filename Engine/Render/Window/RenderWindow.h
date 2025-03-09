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

    const uint32_t GetWidth() const;
    const uint32_t GetHeight() const;
    const HWND& GetHwnd() const;

  private:
    HWND m_hWnd = nullptr;
  };
}



