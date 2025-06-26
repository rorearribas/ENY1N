#include "RenderWindow.h"
#include "Engine/Global/GlobalResources.h"
#include "Engine/Managers/InputManager.h"
#include <windows.h>
#include <cassert>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hWnd, UINT _uMessage, WPARAM _wParam, LPARAM _lParam);

namespace render
{
  namespace internal_renderwindow
  {
    static LRESULT CALLBACK WindowProc(HWND _hWnd, uint32_t _uMessage, WPARAM _wParam, LPARAM _lParam)
    {
      if (ImGui_ImplWin32_WndProcHandler(_hWnd, _uMessage, _wParam, _lParam))
        return true;

      // sort through and find what code to run for the message given
      switch (_uMessage)
      {
      case WM_DESTROY:
      {
        PostQuitMessage(0);
        return 0;
      }
      break;

      case WM_SIZE:
      {
        if (_wParam == SIZE_RESTORED || _wParam == SIZE_MAXIMIZED)
        {
          for (auto& oDelegate : global::delegates::s_vctOnWindowResizeDelegates)
          {
            oDelegate((uint32_t)(LOWORD(_lParam)), (uint32_t)(HIWORD(_lParam)));
          }
        }
      }
      break;

      case WM_INPUT:
      {
        uint32_t uSize = 0;
        GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, nullptr, &uSize, sizeof(RAWINPUTHEADER));
        if (uSize == 0) break;

        // Get raw input data
        std::unique_ptr<BYTE[]> pData(new BYTE[uSize]);
        if (GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, pData.get(), &uSize, sizeof(RAWINPUTHEADER)) != uSize)
        {
          assert(false && "Failed to retrieve RAWINPUT data.");
          break;
        }

        RAWINPUT* pRawInput = reinterpret_cast<RAWINPUT*>(pData.get());
        // Mouse
        if (pRawInput && pRawInput->header.dwType == RIM_TYPEMOUSE)
        {
          global::delegates::s_oUpdateMouseDelegate(&pRawInput->data.mouse);
        }
        // Keyboard
        if (pRawInput && pRawInput->header.dwType == RIM_TYPEKEYBOARD)
        {
          global::delegates::s_oOnUpdateKeyboardDelegate(&pRawInput->data.keyboard);
        }
      }
      break;
      }

      // Handle any messages the switch statement didn't
      return DefWindowProc(_hWnd, _uMessage, _wParam, _lParam);
    }

    static HWND WINAPI CreateWinMain(HINSTANCE hInstance, uint32_t _uWidth, uint32_t _uHeight)
    {
      // this struct holds information for the window class
      WNDCLASSEX wc;

      // clear out the window class for use
      ZeroMemory(&wc, sizeof(WNDCLASSEX));

      // fill in the struct with the needed information
      wc.cbSize = sizeof(WNDCLASSEX);
      wc.style = CS_HREDRAW | CS_VREDRAW;
      wc.lpfnWndProc = internal_renderwindow::WindowProc;
      wc.hInstance = hInstance;
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszClassName = L"ENY1N";

      // register the window class
      RegisterClassEx(&wc);

      // create the window and use the result as the handle
      global::window::s_oHwnd = CreateWindowEx
      (
        NULL,
        L"ENY1N", // name of the window class
        L"ENY1N",// title of the window
        WS_OVERLAPPEDWINDOW, // window style
        0, // x-position of the window
        0, // y-position of the window
        _uWidth, // width of the window
        _uHeight, // height of the window
        NULL, // we have no parent window, NULL
        NULL, // we aren't using menus, NULL
        hInstance, // application handle
        NULL
      ); // used with multiple windows, NULL

      return global::window::s_oHwnd;
    }
  }
  // ------------------------------------
  CRenderWindow::CRenderWindow(uint32_t _uWidth, uint32_t _uHeight)
  {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    m_hWnd = internal_renderwindow::CreateWinMain(hInstance, _uWidth, _uHeight);
    assert(m_hWnd);
  }
  // ------------------------------------
  void CRenderWindow::SetEnabled(bool _bEnabled) const
  {
    ShowWindow(m_hWnd, _bEnabled);
  }
  // ------------------------------------
  const HWND& CRenderWindow::GetHwnd() const
  {
    return m_hWnd;
  }
  // ------------------------------------
  const uint32_t CRenderWindow::GetWidth() const
  {
    RECT oClientRect;
    GetClientRect(m_hWnd, &oClientRect);
    return static_cast<uint32_t>(oClientRect.right - oClientRect.left);
  }
  // ------------------------------------
  const uint32_t CRenderWindow::GetHeight() const
  {
    RECT oClientRect;
    GetClientRect(m_hWnd, &oClientRect);
    return static_cast<uint32_t>(oClientRect.bottom - oClientRect.top);
  }
  // ------------------------------------

}
