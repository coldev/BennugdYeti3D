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

#include "UnitMain.h"
#include "y3d_draw.h"
#include "y3d_quake.h"
#include "y3d_engine.h"
#include "maps.h"
#include "yeti3dwin32.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{

  Caption = Application->Title;

  SL = new TStringList;

  MD2Model = new TMemoryStream;
  YetiModel = new TMemoryStream;

  BMSkin = new Graphics::TBitmap;

  Bitmap = new Graphics::TBitmap;
  Bitmap->PixelFormat = pf16bit;
  Bitmap->Width = YETI_FRAMEBUFFER_WIDTH;
  Bitmap->Height = YETI_FRAMEBUFFER_HEIGHT;

  BMBackDrop = new Graphics::TBitmap;

  BitmapLoadFromTexture(BMBackDrop, textures[60], palette);

}


//---------------------------------------------------------------------------
void ModelConvert(TMemoryStream* Dst, TMemoryStream* Src, Graphics::TBitmap* ASkin)
{
#define WRITE(A) {short n = (A); Dst->Write(&n, sizeof(n));}


  Dst->Clear();
  WRITE(ASkin->Width);
  WRITE(ASkin->Height);


  for (int Y = 0; Y < ASkin->Height; Y++)
  {
    for (int X = 0; X < ASkin->Width; X++)
    {
      int C = ASkin->Canvas->Pixels[X][Y];
      int R = GetRValue(C) * 31 / 255;
      int G = GetGValue(C) * 31 / 255;
      int B = GetBValue(C) * 31 / 255;
      WRITE(RGB_SET(R, G, B));
    }
  }
}
//---------------------------------------------------------------------------
void TFormMain::LoadModel(String AFileName)
{

}

void TFormMain::LoadSkin(String AFileName)
{
  BitmapLoadFromFile(BMSkin, AFileName);
  BMSkin->PixelFormat = pf32bit;
  //BitmapResize(BMSkin, YETI_SKIN_WIDTH, YETI_SKIN_HEIGHT);
  PBSkin->Refresh();

  Edit1->Text = BMSkin->Width ;
  Edit2->Text = BMSkin->Height;
}

void TFormMain::LoadFromFile(String AModelName, String ASkinName)
{
  //LoadModel(AModelName);
  LoadSkin(ASkinName);



  ModelConvert(YetiModel, MD2Model, BMSkin);


}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIOpenClick(TObject *Sender)
{

    String ModelName = OpenDialog->FileName;

    OpenDialog->FileName = "";
    OpenDialog->Title = "Select Sprite Bitmap...";
    OpenDialog->Filter = "Bitmap Files (*.bmp)|*.bmp;*.jpg;*.jpeg";

    if (OpenDialog->Execute())
    {
      LoadFromFile(ModelName, OpenDialog->FileName);
    }

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TFormMain::TBOpenClick(TObject *Sender)
{
  MIOpen->Click();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBSaveClick(TObject *Sender)
{
  MIExportBin->Click();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PBSkinPaint(TObject *Sender)
{
  PBSkin->Canvas->StretchDraw(PBSkin->ClientRect, BMSkin);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExportBinClick(TObject *Sender)
{
  SaveDialog->FileName = "";
  SaveDialog->Title = "Export Sprite File...";
  SaveDialog->DefaultExt = "spr";
  SaveDialog->Filter = "Sprite Files (*.spr)|*.spr";

  if (SaveDialog->Execute())
  {
    YetiModel->SaveToFile(SaveDialog->FileName);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ExportCC1Click(TObject *Sender)
{
  SaveDialog->FileName = "";
  SaveDialog->Title = "Export C/C++ File...";
  SaveDialog->DefaultExt = "c";
  SaveDialog->Filter = "C/C++ Files (*.c)|*.c;*.cpp";

  if (SaveDialog->Execute())
  {
    Application->ProcessMessages();
    String Name = ChangeFileExt(ExtractFileName(SaveDialog->FileName), "");
    ConvertMemoryStreamToStringList(YetiModel, SL, Name);
    SL->SaveToFile(SaveDialog->FileName);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIAboutClick(TObject *Sender)
{
  ShowMessage(YETI_STR_CAPTION);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

