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

#ifndef BCBToolsH
#define BCBToolsH

/******************************************************************************/

#include <Graphics.hpp>
#include <Math.hpp>
#include <JPEG.hpp>

#include "y3d_yeti.h"

#define YETI_GAMMA (2.1)

#define YETI_STR_HEADER \
"/*\n"\
"** Name: Yeti3D\n"\
"** Desc: Portable GameBoy Advanced 3D Engine\n"\
"** Auth: Derek J. Evans <derek@theteahouse.com.au>\n"\
"**\n"\
"** Copyright (C) 2003-2004 Derek J. Evans. All Rights Reserved.\n"\
"**\n"\
"** YY  YY EEEEEE TTTTTT IIIIII 33333  DDDDD\n"\
"** YY  YY EE       TT     II       33 DD  DD\n"\
"**  YYYY  EEEE     TT     II     333  DD  DD\n"\
"**   YY   EE       TT     II       33 DD  DD\n"\
"**   YY   EEEEEE   TT   IIIIII 33333  DDDDD\n"\
"*/\n"

/******************************************************************************/

extern TRect RectNormalize(TRect* ARect);
extern TRect RectInflate(TRect* ARect, int X, int Y);

extern void ConvertMemoryStreamToStringList(TMemoryStream* Src, TStringList* Dst, String AName);
extern void BitmapResize(Graphics::TBitmap* ABitmap, int AWidth, int AHeight);
extern void BitmapLoadFromFile(Graphics::TBitmap* ABitmap, String AFileName);
extern void BitmapLoadFromTexture(Graphics::TBitmap* ABitmap, texture_t texture, palette_t palette);
extern void BitmapDrawViewport(Graphics::TBitmap* ABitmap, viewport_t* viewport);

/******************************************************************************/

#endif

