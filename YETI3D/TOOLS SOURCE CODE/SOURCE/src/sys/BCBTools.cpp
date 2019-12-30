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

#include "BCBTools.h"
#include "y3d_pixel.h"

/******************************************************************************/

TRect RectNormalize(TRect* ARect)
{
  return Rect(
    Min((int)ARect->Left, ARect->Right),
    Min((int)ARect->Top , ARect->Bottom),
    Max((int)ARect->Left, ARect->Right),
    Max((int)ARect->Top , ARect->Bottom)
    );
}

TRect RectInflate(TRect* ARect, int X, int Y)
{
  return Rect(
    ARect->Left - X,
    ARect->Top - Y,
    ARect->Right + X,
    ARect->Bottom + Y
    );
}

/******************************************************************************/

void ConvertMemoryStreamToStringList(TMemoryStream* Src, TStringList* Dst, String AName)
{
  String S;
  Byte* P = (byte*) Src->Memory;

  Dst->Clear();
  Dst->Add(YETI_STR_HEADER);
  Dst->Add("const unsigned char " + AName + "[] = {");

  for (int I = Src->Size; I--;)
  {
    S += (int)*P++;
    if (I) S += ",";
    if (S.Length() > 70 || !I)
    {
      Dst->Add(S);
      S = "";
    }
  }
  Dst->Add("};");
}


/******************************************************************************/

/*
** Name: BitmapResize
** Desc:
*/
void BitmapResize(Graphics::TBitmap* ABitmap, int AWidth, int AHeight)
{
  Graphics::TBitmap* Temp = new Graphics::TBitmap;
  try
  {
    Temp->PixelFormat = ABitmap->PixelFormat;
    Temp->Width = AWidth;
    Temp->Height = AHeight;
    Temp->Canvas->StretchDraw(Bounds(0, 0, AWidth, AHeight), ABitmap);
    ABitmap->Width = AWidth;
    ABitmap->Height = AHeight;
    ABitmap->Canvas->Draw(0, 0, Temp);
  }
  __finally
  {
    Temp->Free();
  }
}

/*
** Name: BitmapLoadFromFile
** Desc:
*/
void BitmapLoadFromFile(Graphics::TBitmap* ABitmap, String AFileName)
{
  String Ext = ExtractFileExt(AFileName);

  if (!Ext.AnsiCompareIC(".jpg") || !Ext.AnsiCompareIC(".jpeg"))
  {
    TJPEGImage* JPEGImage = new TJPEGImage;
    try
    {
      JPEGImage->LoadFromFile(AFileName);
      ABitmap->Assign(JPEGImage);
    }
    __finally
    {
      JPEGImage->Free();
    }
  }
  if (!Ext.AnsiCompareIC(".bmp"))
  {
    ABitmap->LoadFromFile(AFileName);
  }
}

/*
** Name: BitmapLoadFromTexture
** Desc:
*/
void BitmapLoadFromTexture(Graphics::TBitmap* ABitmap, texture_t texture, palette_t palette)
{
  int x, y;

  ABitmap->PixelFormat = pf32bit;
  ABitmap->Width  = TEXTURE_WIDTH;
  ABitmap->Height = TEXTURE_HEIGHT;

  for (y = TEXTURE_HEIGHT; y--;)
  {
    for (x = TEXTURE_WIDTH; x--;)
    {
      int c = texture[y][x];

      ((u32*)ABitmap->ScanLine[y])[x] = RGB(palette[c][2], palette[c][1], palette[c][0]);
    }
  }
}

/*
** Name: BitmapDrawViewport
** Desc:
*/
void BitmapDrawViewport(Graphics::TBitmap* ABitmap, viewport_t* viewport)
{
  static pixel_converter_t pixel_converter;

  if (!pixel_converter.length)
  {
    pixel_converter_init(&pixel_converter, 31 << 11, 31 << 6, 31 << 0, 0);
  }

  pixel_buffer_draw(
    viewport->back,
    (u16*) ABitmap->ScanLine[0],
    (int) ABitmap->ScanLine[1] - (int) ABitmap->ScanLine[0],
    &pixel_converter);
}
