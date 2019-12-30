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

#ifndef UnitMainH
#define UnitMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Math.hpp>
#include <JPEG.hpp>

#include "game.h"
#include "BCBTools.h"
#include <ImgList.hpp>
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
  TStatusBar *StatusBar;
  TToolBar *ToolBar;
  TToolButton *TBNew;
  TToolButton *TBOpen;
  TToolButton *TBSave;
  TMainMenu *MainMenu;
  TMenuItem *MIFile;
  TMenuItem *MIOpen;
  TOpenDialog *OpenDialog;
  TImageList *ImageList;
  TPanel *PanelLeft;
  TGroupBox *GBMD2Properties;
  TPanel *LabelVersion;
  TEdit *Edit2;
  TEdit *Edit1;
  TGroupBox *GBSkinImage;
  TSaveDialog *SaveDialog;
  TMenuItem *N1;
  TMenuItem *MIExit;
  TMenuItem *N2;
  TMenuItem *MIExportBin;
  TMenuItem *MIHelp;
  TMenuItem *MIAbout;
  TPanel *Panel2;
  TMenuItem *ExportCC1;
        TPaintBox *PBSkin;
  void __fastcall FormCreate(TObject *Sender);


  void __fastcall MIOpenClick(TObject *Sender);
  void __fastcall TBOpenClick(TObject *Sender);
  void __fastcall PBSkinPaint(TObject *Sender);

  void __fastcall MIExitClick(TObject *Sender);
  void __fastcall MIExportBinClick(TObject *Sender);
  void __fastcall MIAboutClick(TObject *Sender);
  void __fastcall TBSaveClick(TObject *Sender);


 
  void __fastcall ExportCC1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFormMain(TComponent* Owner);

  int Zoom;
  TPoint MDP;
  Graphics::TBitmap* BMBackDrop;
  Graphics::TBitmap* BMSkin;
  Graphics::TBitmap* Bitmap;
  TStringList* SL;
  entity_t* entity;
  TMemoryStream* MD2Model;
  TMemoryStream* YetiModel;
  pixel_buffer_t framebuffer;
  yeti_t yeti;
  void LoadSkin(String AFileName);
  void LoadModel(String AFileName);
  void LoadFromFile(String AModelName, String ASkinName);

};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
