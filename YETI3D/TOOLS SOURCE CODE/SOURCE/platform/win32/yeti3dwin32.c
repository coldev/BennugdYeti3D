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

#include "yeti3dwin32.h"

/*
** Name: keyboard_update
** Desc: Updates a keyboard structure with the latest keyboard states. These
**       are standard Quake keys.
** Note: Win32 only.
*/
#define ISKEYDOWN(A) (GetAsyncKeyState(A)<0)

void keyboard_update(keyboard_t* kb)
{
  kb->state.up      = ISKEYDOWN(VK_UP);
  kb->state.down    = ISKEYDOWN(VK_DOWN);
  kb->state.left    = ISKEYDOWN(VK_LEFT);
  kb->state.right   = ISKEYDOWN(VK_RIGHT);
  kb->state.a       = ISKEYDOWN(VK_CONTROL);
  kb->state.select  = ISKEYDOWN(VK_RETURN);
  kb->state.b       = ISKEYDOWN(' ');
  kb->state.l       = ISKEYDOWN('A');
  kb->state.r       = ISKEYDOWN('Z') || ISKEYDOWN('Y'); // Support for Germans KB's
  kb->state.rotate  = ISKEYDOWN('R');

  GetCursorPos((PPOINT)&kb->state.mouse);
}

/*
** Name: wglSetDisplayMode
** Desc:
*/
void wglSetDisplayMode(int width, int height, int bpp)
{
  DEVMODE dm;

  dm.dmSize       = sizeof(dm);
  dm.dmPelsWidth  = width;
  dm.dmPelsHeight = height;
  dm.dmBitsPerPel = bpp;
  dm.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
  
  ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

  ShowCursor(0);  
}

/*
** Name: wglResetDisplaymode
** Desc:
*/
void wglResetDisplaymode(void)
{
  ChangeDisplaySettings(NULL, CDS_FULLSCREEN);
}

