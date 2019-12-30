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

#ifndef UnitPreviewH
#define UnitPreviewH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "game.h"
#include "y3d_draw.h"
//---------------------------------------------------------------------------
class TFormPreview : public TForm
{
__published:	// IDE-managed Components
  TTimer *Timer;
  void __fastcall TimerTimer(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
  void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
public:		// User declarations
  pixel_buffer_t framebuffer;
  yeti_t yeti;
  Graphics::TBitmap* Bitmap;
  TPoint MDP;
  __fastcall TFormPreview(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormPreview *FormPreview;
//---------------------------------------------------------------------------
#endif
