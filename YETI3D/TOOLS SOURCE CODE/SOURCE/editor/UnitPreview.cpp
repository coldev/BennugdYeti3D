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

#include <vcl.h>
#pragma hdrstop

#include "UnitPreview.h"
#include "UnitMain.h"
#include "yeti3dwin32.h"
#include "maps.h"
#include "y3d_file.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormPreview *FormPreview;
//---------------------------------------------------------------------------
__fastcall TFormPreview::TFormPreview(TComponent* Owner): TForm(Owner){}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormCreate(TObject *Sender)
{
  Timer->Interval = YETI_VIEWPORT_INTERVAL;
  Timer->Enabled = True;
  
  Bitmap = new Graphics::TBitmap;

  Bitmap->PixelFormat = pf16bit;
  Bitmap->Width  = YETI_FRAMEBUFFER_WIDTH;
  Bitmap->Height = YETI_FRAMEBUFFER_HEIGHT;

  ClientWidth  = Bitmap->Width;
  ClientHeight = Bitmap->Height;
             
  SetBounds(
    Screen->DesktopWidth - Width - 16,
    Screen->DesktopHeight - Height - 16,
    Width, Height);

  yeti_init(&yeti,
    framebuffer.pixels[0],
    framebuffer.pixels[0],
    textures,
    palette);

  yeti_init_lua(&yeti, YETI_GAMMA);

  game_load_map(&yeti, &map_e2m3);
  game_goto(&yeti.game, GAME_MODE_PLAY);

  FormMain->yeti = &yeti;
}
//---------------------------------------------------------------------------
int xcurrent=-1,ycurrent=-1;

void __fastcall TFormPreview::TimerTimer(TObject *Sender)
{
  if (xcurrent < 0) {xcurrent= yeti.camera->x; ycurrent= yeti.camera->y;}

  if (Application->Active)
  {
    //pixel_buffer_clear(yeti.viewport.back);
    keyboard_update(&yeti.keyboard);
    if (yeti.keyboard.state.a)
    {
      yeti.keyboard.state.a = FALSE;
      yeti.keyboard.state.l = FALSE;
      yeti.keyboard.state.r = FALSE;
    }
    yeti.keyboard.state.select = FALSE;

    if ((f2i(yeti.camera->x)>62)||
       (f2i(yeti.camera->x)<1) ||
       (f2i(yeti.camera->y)>62)||
       (f2i(yeti.camera->y)<1))
    {
     yeti.camera->x= xcurrent; yeti.camera->y= ycurrent;
    }
    else
    {
      xcurrent= yeti.camera->x; ycurrent= yeti.camera->y;
    }


    game_loop(&yeti);
    BitmapDrawViewport(Bitmap, &yeti.viewport);
    FormPaint(this);
    FormMain->PanelCamera->SetBounds(
      f2fl(yeti.camera->x) * FormMain->Zoom - 3,
      (MAP_HEIGHT - f2fl(yeti.camera->y) - 0) * FormMain->Zoom - 3,
      7, 7);

    FormMain->ScrollBox->ScrollInView(FormMain->PanelCamera);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormPaint(TObject *Sender)
{
  Canvas->Draw(0, 0, Bitmap);
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  FormMain->OnKeyDown(FormMain, Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TFormPreview::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  FormMain->OnKeyUp(FormMain, Key, Shift);
}
//---------------------------------------------------------------------------

void __fastcall TFormPreview::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft)
  {
    MDP = Point(X, Y);
    Screen->Cursor = crSize;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormPreview::FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  if (Screen->Cursor == crSize)
  {
    SetBounds(
      Left + (X - MDP.x),
      Top  + (Y - MDP.y),
      Width, Height);
  }
}
//---------------------------------------------------------------------------

void __fastcall TFormPreview::FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft)
  {
    Screen->Cursor = crDefault;
  }
}
//---------------------------------------------------------------------------


