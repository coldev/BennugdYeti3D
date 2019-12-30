/*
** Name: Yeti3D
** Desc: Portable GameBoy Advanced 3D Engine
** Auth: Derek J. Evans <derek@theteahouse.com.au>
**
** Copyright (C) 2003-2004 Derek J. Evans. All Rights Reserved.
**
** YY  YY EEEEEE TTTTTT IIIIII 33333  DDDDD
** YY  YY EE       TT     II       33 DD  DD
**  YYYY  EEEE     TT     II     333  DD  DD
**   YY   EE       TT     II       33 DD  DD
**   YY   EEEEEE   TT   IIIIII 33333  DDDDD
*/

#include <windows.h>
#include <mmsystem.h>

#include "yeti3dwin32.h"
#include "Charles/charles.h"

char* lpszClassName = "Yeti3D";
char* lpszCaption = YETI_STR_TITLE;
char* lpszMessage =
  YETI_STR_TITLE " " YETI_STR_VERSION " (Win32 Version)\n"
  "Compilied using Watcom C/C++ v11 (Release Build)\n\n"
  YETI_STR_COPYRIGHT "\n"
  "Website: " YETI_STR_URL "\n";

HDIB* dib;
framebuffer_t framebuffer;
yeti_t yeti;

WNDCLASS wc;
MSG msg;
HWND hWnd;
 
void DibDraw(HDC hdc, int x, int y, HDIB dib)
{
  HDC hdcMem = CreateCompatibleDC(hdc);
  HANDLE oldBitmap = SelectObject(hdcMem, DibBitmapHandle(dib));
  BitBlt (hdc, x, y, x + DibWidth(dib), y + DibHeight(dib), hdcMem, 0, 0, SRCCOPY);
  SelectObject(hdcMem, oldBitmap);
  DeleteDC(hdcMem);    
}
 
void d3d_flip(void)
{
  HDC dc;
  static unsigned MarkTime;

  if ((int)(MarkTime - timeGetTime()) < 0)
  {
    MarkTime = timeGetTime() + YETI_VIEWPORT_INTERVAL;   

    keyboard_update(&yeti.keyboard);

    game_loop(&yeti);
       
    viewport_to_video(
      (u16*) DibPixelPtr(dib, 0, 0),
      (int)DibPixelPtr(dib, 0, 1) - (int)DibPixelPtr(dib, 0, 0),
      &yeti.viewport,
      31 << 10,
      31 << 5,
      31);
    
    dc = GetDC(hWnd);
    
    if (dc)
    {   
      DibDraw(dc, 0, 0, dib);
      ReleaseDC(hWnd, dc);
    }
  }
}

LRESULT CALLBACK WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_KEYDOWN:
    {
      switch (wParam)
      {
        case VK_ESCAPE:
        {
          DestroyWindow(hWnd);
          break;
        }
      }
      break;
    }
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
    default:
    {
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
  }
  return 0;
}

WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  MessageBox(0, lpszMessage, lpszCaption, MB_OK);
  lpszMessage = 0;
  
  wc.style          = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc    = WindowProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = sizeof(DWORD);
  wc.hInstance      = hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = NULL;
  wc.hbrBackground  = GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName   = NULL;
  wc.lpszClassName  = lpszClassName;

  if (!RegisterClass(&wc)) lpszMessage = "Could not register class.";
  else
  {    
    hWnd = CreateWindow(
      lpszClassName,
      lpszCaption,
      WS_VISIBLE | WS_POPUPWINDOW,
      0, //(GetSystemMetrics(SM_CXSCREEN) - YETI_VIEWPORT_WIDTH) >> 1,
      0, //(GetSystemMetrics(SM_CYSCREEN) - YETI_VIEWPORT_HEIGHT) >> 1,
      YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT,
      NULL,
      NULL,
      hInstance,
      NULL);

    if (!hWnd) lpszMessage = "Could not create window.";
    else
    {
      ShowWindow(hWnd, nCmdShow);
      UpdateWindow(hWnd);
      wglSetDisplayMode(YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT, 16);

      dib = DibCreate(YETI_VIEWPORT_WIDTH, YETI_VIEWPORT_HEIGHT, 16, 0);
        
      yeti_init(&yeti, &framebuffer, &framebuffer, textures, palette, lua0, lua1, lua2, lua3);
      game_init(&yeti);

      while (msg.message != WM_QUIT)
      {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
        else
        {
          d3d_flip();
        }
      }
    }
  }
  if (lpszMessage)
  {
    MessageBox(0, lpszMessage, lpszCaption, MB_OK);
  }
            
  return msg.wParam;
}


