#include "RenderWindow.h"
#include <windows.h>
#include <windowsx.h>

namespace render
{
  namespace internal_renderwindow
  {
    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      // sort through and find what code to run for the message given
      switch (message)
      {
        // this message is read when the window is closed
      case WM_DESTROY:
      {
        // close the application entirely
        PostQuitMessage(0);
        return 0;
      } break;
      }

      // Handle any messages the switch statement didn't
      return DefWindowProc(hWnd, message, wParam, lParam);
    }

    HWND WINAPI CreateWinMain(HINSTANCE hInstance, const UINT32& _uWidth, const UINT32& _uHeight)
    {
      // the handle for the window, filled by a function
      HWND hWnd;
      // this struct holds information for the window class
      WNDCLASSEX wc;

      // clear out the window class for use
      ZeroMemory(&wc, sizeof(WNDCLASSEX));

      // fill in the struct with the needed information
      wc.cbSize = sizeof(WNDCLASSEX);
      wc.style = CS_HREDRAW | CS_VREDRAW;
      wc.lpfnWndProc = WindowProc;
      wc.hInstance = hInstance;
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
      wc.lpszClassName = L"WindowClass1";

      // register the window class
      RegisterClassEx(&wc);

      // create the window and use the result as the handle
      hWnd = CreateWindowEx(NULL,
        L"WindowClass1",    // name of the window class
        L"Game Engine",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style
        _uWidth / 2,    // x-position of the window
        _uHeight / 2,    // y-position of the window
        _uWidth,    // width of the window
        _uHeight,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        hInstance,    // application handle
        NULL);    // used with multiple windows, NULL

      return hWnd;
    }
  }

  void CRenderWindow::CreateRenderWindow(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    m_hWnd = internal_renderwindow::CreateWinMain(hInstance, _uWidth, _uHeight);
  }

  void CRenderWindow::ShowRenderWindow()
  {
    ShowWindow(m_hWnd, true);
  }

  void render::CRenderWindow::UpdateWindow()
  {
    // this struct holds Windows event messages
    MSG msg;

    // wait for the next message in the queue, store the result in 'msg'
    while (GetMessage(&msg, NULL, 0, 0))
    {
      // translate keystroke messages into the right format
      TranslateMessage(&msg);

      // send the message to the WindowProc function
      DispatchMessage(&msg);
    }
  }
}
