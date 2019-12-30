/*
** Name: Yeti3D PRO
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

#include "UnitSplash.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormSplash *FormSplash;
//---------------------------------------------------------------------------
__fastcall TFormSplash::TFormSplash(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void TFormSplash::Execute()
{
  String LFileName = ExtractFilePath(Application->ExeName) + "splash.bmp";

  if (FileExists(LFileName))
  {
    Image->Picture->LoadFromFile(LFileName);
    ClientWidth = Image->Picture->Width;
    ClientHeight = Image->Picture->Height;
    SetBounds(
      (Screen->Width - Width) / 2,
      (Screen->Height - Height) / 2,
      Width, Height);
    Timer->Enabled = True;
    Show();
    Update();
    Application->ProcessMessages();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormSplash::TimerTimer(TObject *Sender)
{
  Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormSplash::FormDeactivate(TObject *Sender)
{
  Hide();
}
//---------------------------------------------------------------------------
void __fastcall TFormSplash::FormHide(TObject *Sender)
{
  Timer->Enabled = False;
  Image->Picture->Assign(NULL);
}
//---------------------------------------------------------------------------

