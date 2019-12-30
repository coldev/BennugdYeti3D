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

#ifndef UnitMainH
#define UnitMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <ImgList.hpp>
#include <Dialogs.hpp>
#include <Math.hpp>
#include <StrUtils.hpp>
#include <ExtDlgs.hpp>
#include <IniFiles.hpp>
#include "BCBTools.h"
//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
  TStatusBar *StatusBar;
  TToolBar *ToolBar;
  TPanel *PanelClient;
  TScrollBox *ScrollBox;
  TMainMenu *MainMenu;
  TMenuItem *MIFile;
  TMenuItem *MINew;
  TMenuItem *MIOpen;
  TMenuItem *N1;
  TMenuItem *MISave;
  TMenuItem *MISaveAs;
  TMenuItem *N2;
  TMenuItem *MIExit;
  TMenuItem *MIEdit;
  TMenuItem *MIHelp;
  TToolButton *TBNew;
  TToolButton *TBOpen;
  TToolButton *TBSave;
  TToolButton *ToolButton4;
  TToolButton *TBCut;
  TToolButton *TBCopy;
  TToolButton *TBPaste;
  TToolButton *TBDelete;
  TToolButton *ToolButton9;
  TToolButton *TBArrow;
  TToolButton *TBDraw;
  TPanel *PanelScene;
  TPaintBox *PaintBox;
  TMenuItem *MIAbout;
  TImageList *ImageList;
  TSaveDialog *ExportDialog;
  TMenuItem *N3;
  TMenuItem *ExportCFile;
  TSaveDialog *SaveDialog;
  TOpenDialog *OpenDialog;
  TPanel *PanelCamera;
  TMenuItem *MIUndo;
  TMenuItem *MIRedo;
  TMenuItem *N4;
  TMenuItem *MICut;
  TMenuItem *MICopy;
  TMenuItem *MIPaste;
  TMenuItem *MIDelete;
  TMenuItem *N5;
  TMenuItem *MISelectAll;
  TSplitter *Splitter;
  TImageList *ILTextures;
  TToolButton *ToolButton1;
  TToolButton *ToolButton2;
  TToolButton *ToolButton3;
  TPaintBox *PBMouse;
  TMenuItem *N6;
  TMenuItem *MIImportTextures;
  TOpenPictureDialog *OpenPictureDialog;
  TTimer *Timer;
  TPageControl *PageControl;
  TTabSheet *TabSheet1;
  TListView *ListView;
        TMenuItem *MIExportTexturesPalette;
        TSaveDialog *SaveDialogText;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall PaintBoxPaint(TObject *Sender);
  void __fastcall ScrollBoxResize(TObject *Sender);
  void __fastcall MIExitClick(TObject *Sender);
  void __fastcall PaintBoxMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall PaintBoxMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall PaintBoxMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall ExportCFileClick(TObject *Sender);
  void __fastcall MISaveAsClick(TObject *Sender);
  void __fastcall MIOpenClick(TObject *Sender);
  void __fastcall MINewClick(TObject *Sender);
  void __fastcall TBNewClick(TObject *Sender);
  void __fastcall TBOpenClick(TObject *Sender);
  void __fastcall TBSaveClick(TObject *Sender);
  void __fastcall MISaveClick(TObject *Sender);
  void __fastcall MIAboutClick(TObject *Sender);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall ToolButton2Click(TObject *Sender);
  void __fastcall ToolButton3Click(TObject *Sender);
  void __fastcall ListViewChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
  void __fastcall MISelectAllClick(TObject *Sender);
  void __fastcall ListViewMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
  void __fastcall MICopyClick(TObject *Sender);
  void __fastcall MIDeleteClick(TObject *Sender);
  void __fastcall MICutClick(TObject *Sender);
  void __fastcall MIImportTexturesClick(TObject *Sender);
  void __fastcall TimerTimer(TObject *Sender);
        void __fastcall MIExportTexturesPaletteClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFormMain(TComponent* Owner);

  bool tc;
  bool Modified;
  DynamicArray<DynamicArray<rom_cell_t> > MapClip;
  Graphics::TBitmap* TmpBitmap;
  String FileName;
  rom_map_t rommap;
  yeti_t* yeti;
  bool LButton;
  bool RButton;
  int Zoom;
  TRect Selection;
  TRect SelectionStatus;

  void UpdateTextureList();
  void SetZoom(int AZoom);
  TRect CellRect(int x, int y);
  void CellDraw(int x, int y);
  void SetFileName(String AFileName);
  void SaveToFile(String AFileName);
  void LoadFromFile(String AFileName);
  void ExportToFile(String AFileName);
  bool CanClose();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
