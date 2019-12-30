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
  TScrollBox *ScrollBox;
  TMainMenu *MainMenu;
  TMenuItem *MIFile;
  TMenuItem *MIOpen;
  TOpenDialog *OpenDialog;
  TTimer *Timer;
  TPanel *PanelScene;
  TPaintBox *PBScene;
  TImageList *ImageList;
  TToolButton *ToolButton4;
  TToolButton *TBZoomIn;
  TToolButton *TBZoomOut;
  TPanel *PanelLeft;
  TGroupBox *GBMD2Properties;
  TPanel *LabelVersion;
  TPanel *LabelNumTriangles;
  TPanel *LabelNumVertices;
  TEdit *Edit3;
  TEdit *Edit2;
  TEdit *Edit1;
  TGroupBox *GBSkinImage;
  TPaintBox *PBBackDrop;
  TPanel *LabelTexCoords;
  TEdit *Edit4;
  TEdit *Edit5;
  TPanel *LabelNumFrames;
  TSaveDialog *SaveDialog;
  TMenuItem *N1;
  TMenuItem *MIExit;
  TMenuItem *N2;
  TMenuItem *MIExportBin;
  TMenuItem *MIHelp;
  TMenuItem *MIAbout;
  TPanel *PanelSkin;
  TPaintBox *PBSkin;
  TGroupBox *GBFrameSettings;
  TTrackBar *TBFrame;
  TCheckBox *CBAnimate;
  TPanel *Panel1;
  TEdit *Edit6;
  TPanel *Panel2;
  TEdit *Edit7;
  TMenuItem *ExportCC1;
        TEdit *Edit8;
        TPanel *Panel3;
        TPanel *Panel4;
        TEdit *Edit9;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall PBScenePaint(TObject *Sender);
  void __fastcall ScrollBoxResize(TObject *Sender);
  void __fastcall MIOpenClick(TObject *Sender);
  void __fastcall TimerTimer(TObject *Sender);
  void __fastcall TBZoomInClick(TObject *Sender);
  void __fastcall TBZoomOutClick(TObject *Sender);
  void __fastcall TBOpenClick(TObject *Sender);
  void __fastcall PBSkinPaint(TObject *Sender);
  void __fastcall PBBackDropPaint(TObject *Sender);
  void __fastcall MIExitClick(TObject *Sender);
  void __fastcall MIExportBinClick(TObject *Sender);
  void __fastcall MIAboutClick(TObject *Sender);
  void __fastcall TBSaveClick(TObject *Sender);
  void __fastcall TBFrameChange(TObject *Sender);
  void __fastcall PBSceneMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall PBSceneMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall ExportCC1Click(TObject *Sender);
        void __fastcall Edit9Change(TObject *Sender);
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
  void SetZoom(int AZoom);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
