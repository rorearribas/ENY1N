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
    void GetWindowSize(uint32_t& _uWidth, uint32_t& _uHeight) const;
    inline const HWND& GetHandle() const { return m_hHandle; }

  private:
    HWND m_hHandle = nullptr;
  };
}



