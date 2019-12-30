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

#ifndef __YETI3DWIN32_H__
#define __YETI3DWIN32_H__

/******************************************************************************/

#include <windows.h>
#include "y3d_keyboard.h"

/******************************************************************************/

#ifdef __cplusplus
extern "C"{
#endif

/******************************************************************************/

void keyboard_update(keyboard_t* kb);
void wglSetDisplayMode(int width, int height, int bpp);
void wglResetDisplaymode(void);

/******************************************************************************/

#ifdef __cplusplus
};
#endif

/******************************************************************************/

#endif
 
