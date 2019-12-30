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

#ifndef UnitSplashH
#define UnitSplashH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TFormSplash : public TForm
{
__published:	// IDE-managed Components
  TTimer *Timer;
  TImage *Image;
  void __fastcall TimerTimer(TObject *Sender);
  void __fastcall FormDeactivate(TObject *Sender);
  void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TFormSplash(TComponent* Owner);

  void Execute();
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSplash *FormSplash;
//---------------------------------------------------------------------------
#endif
