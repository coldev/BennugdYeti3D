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

#include "UnitMain.h"
#include "UnitPreview.h"
#include "UnitSplash.h"
#include "map.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner) : TForm(Owner) {}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCreate(TObject *Sender)
{
  TmpBitmap = new Graphics::TBitmap;

  SelectionStatus = Rect(-1, -1, -1, -1);
  
  SetFileName("");
  PaintBox->ControlStyle = PaintBox->ControlStyle << csOpaque;
  SetZoom(11);

  UpdateTextureList();  
}
//---------------------------------------------------------------------------
void TFormMain::UpdateTextureList()
{
  ListView->Items->BeginUpdate();
  ListView->Clear();
  ILTextures->Clear();

  TmpBitmap->PixelFormat = pf32bit;
  TmpBitmap->Width  = TEXTURE_WIDTH;
  TmpBitmap->Height = TEXTURE_HEIGHT;

  for (int i = 0; i < TEXTURE_MAX; i++)
  {
    for (int y = 0; y < TEXTURE_HEIGHT; y++)
    {
      for (int x = 0; x < TEXTURE_WIDTH; x++)
      {
        int c = textures[i][TEXTURE_HEIGHT - y - 1][x];
        
        ((DWORD*)TmpBitmap->ScanLine[y*64/TEXTURE_HEIGHT])[x*64/TEXTURE_WIDTH] =
          RGB(palette[c][2], palette[c][1], palette[c][0]);
      }
    }
    TListItem* LListItem = ListView->Items->Add();
    LListItem->Caption = String("#") + i;
    LListItem->ImageIndex = ILTextures->Add(TmpBitmap, NULL);
  }

  ListView->ItemIndex = 0;

  ListView->Items->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormPaint(TObject *Sender)
{
  OnPaint = NULL;
  //FormPreview->SetFocus();
  FormSplash->Execute();
}
//---------------------------------------------------------------------------
void TFormMain::SetZoom(int AZoom)
{
  Zoom = EnsureRange(AZoom, 6, 24);
  ScrollBox->OnResize(ScrollBox);
}
//---------------------------------------------------------------------------
TRect TFormMain::CellRect(int x, int y)
{
  return Bounds(x * Zoom, (MAP_HEIGHT - y - 1) * Zoom, Zoom, Zoom);
}
//---------------------------------------------------------------------------
void TFormMain::CellDraw(int x, int y)
{
  TCanvas* LCanvas = PaintBox->Canvas;
  cell_t* cell = &yeti->map.item[y][x];

  TRect r = CellRect(x, y);

  if (CELL_IS_SOLID(cell))
  {
    LCanvas->Brush->Color = (TColor) RGB(0, 0, 150);
  }
  else
  {
    int s = ((cell->top - cell->bot) >> 4);

    LCanvas->Brush->Color = (TColor) RGB(s, s, s);
  }

  TRect s = RectNormalize(&Selection);

  if (x >= s.Left && y >= s.Top && x <= s.Right && y <= s.Bottom)
  {
    cell->swi |= CELL_SWI_SELECTED;

    if (cell->swi & CELL_SWI_TERRAIN || cell->swi & CELL_SWI_CAVE)
    {
      LCanvas->Pen->Color = clLime;
    }
    else
    {
      LCanvas->Pen->Color = clGreen;
    }
    LCanvas->Rectangle(r);

    if (x == Selection.left && y == Selection.top)
    {
      LCanvas->Pen->Color = clRed;
      LCanvas->Rectangle(RectInflate(&r, -2, -2));
    }
  }
  else
  {
    cell->swi &= ~CELL_SWI_SELECTED;

    if (CELL_IS_SOLID(cell))
    {
      LCanvas->FillRect(r);
      TRect tmp = r; // Frame3D modifies the rectangle. 
      Frame3D(LCanvas, tmp, (TColor) RGB(0, 0, 200), (TColor) RGB(0, 0, 100), 1);
      //Frame3D(PaintBox->Canvas, tmp, (TColor) RGB(0, 0, 100), (TColor) RGB(0, 0, 200), 1);
    }
    else
    {
      LCanvas->Pen->Color = clBlack;
      LCanvas->Rectangle(r);
    }
  }

  if (cell->swi & CELL_SWI_LIGHT)
  {
    LCanvas->Pen->Color = clWhite;
    LCanvas->Rectangle(RectInflate(&r, -4, -4));
  }
  
  if (cell->ent)
  {
    if (cell->ent == 1)
    {
      LCanvas->Pen->Color = clYellow;
    }
    else
    {
      LCanvas->Pen->Color = clLime;
    }
    LCanvas->Brush->Style = bsClear;
    LCanvas->Ellipse(RectInflate(&r, -1, -1));
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxPaint(TObject *Sender)
{
  TRect LClipRect = PaintBox->Canvas->ClipRect;

  for (int y = MAP_HEIGHT; y--;)
  {
    for (int x = MAP_WIDTH; x--;)
    {
      TRect LTempRect;
      if (IntersectRect(LTempRect, LClipRect, CellRect(x, y)))
      {
        CellDraw(x, y);
      }
    }
  }
  if (Selection != SelectionStatus)
  {
    SelectionStatus = Selection;
    
    StatusBar->Panels->Items[0]->Text = Format("Selected Cell: (%d, %d)", ARRAYOFCONST((
      (int)Selection.left,
      (int)Selection.top,
      (int)Selection.Width(),
      (int)Selection.Height())));
      
    StatusBar->Update();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ScrollBoxResize(TObject *Sender)
{
  int LWidth  = MAP_WIDTH  * Zoom;
  int LHeight = MAP_HEIGHT * Zoom;

  PanelScene->SetBounds(
    Max((ScrollBox->ClientWidth  - LWidth) / 2, 0) - ScrollBox->HorzScrollBar->Position,
    Max((ScrollBox->ClientHeight - LHeight) / 2, 0) - ScrollBox->VertScrollBar->Position,
    LWidth, LHeight);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExitClick(TObject *Sender)
{
  Close();
}

void cell_solid_switch(cell_t* cell, int issolid)
{
  if (issolid)
  {
    if (!CELL_IS_SOLID(cell))
    {
      cell->tsa = cell->top;
      cell->bsa = cell->bot;
      cell->top = cell->bot = 0;
    }
  }
  else
  {
    if (CELL_IS_SOLID(cell))
    {
      cell->top = cell->tsa;
      cell->bot = cell->bsa;
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if (LButton || RButton)
  {
    PBMouse->SetBounds(X, Y, 0, 0);
    ScrollBox->ScrollInView(PBMouse);
  }

  X = X / Zoom;
  Y = MAP_HEIGHT - (Y / Zoom) - 1;

  //X = EnsureRange(X, 0, YETI_MAP_WIDTH  - 1);
  //Y = EnsureRange(Y, 0, YETI_MAP_HEIGHT - 1);

  if (TBDraw->Down)
  {
    if (LButton || RButton)
    {
      cell_solid_switch(&yeti->map.item[Y][X], LButton);

      map_precalc_data(&yeti->map, yeti->visual.map_lod);

      CellDraw(X, Y);
      Modified = True;
    }
  }
  if (TBArrow->Down && LButton)
  {
    Selection.Right  = X;
    Selection.Bottom = Y;
    PaintBox->Repaint();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft ) LButton = True;
  if (Button == mbRight) RButton = True;

  if (TBArrow->Down)
  {
    Selection = Bounds(X / Zoom, MAP_HEIGHT - (Y / Zoom) - 1, 0, 0);
  }
  PaintBoxMouseMove(Sender, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PaintBoxMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (Button == mbLeft ) LButton = False;
  if (Button == mbRight) RButton = False;

  PaintBox->Repaint();
}
//---------------------------------------------------------------------------
void yeti_lua_init(lua_t lua, palette_t palette,
  float LR, float LG, float LB,
  float pgamma, float sgamma)
{
  int x, y;
  float r, g, b;

  for (y = 0; y < 64; y++)
  {
    for (x = 0; x < 256; x++)
    {
      r = palette[x][0];
      g = palette[x][1];
      b = palette[x][2];

      r = pow(r / 255.0, pgamma) * 255.0;
      g = pow(g / 255.0, pgamma) * 255.0;
      b = pow(b / 255.0, pgamma) * 255.0;

      r = r * LR;
      g = g * LG;
      b = b * LB;

      r = r * y / 32;
      g = g * y / 32;
      b = b * y / 32;

      r = pow(r / 255.0, sgamma) * 255.0;
      g = pow(g / 255.0, sgamma) * 255.0;
      b = pow(b / 255.0, sgamma) * 255.0;

      r = r / 255.0 * 31.0;
      g = g / 255.0 * 31.0;
      b = b / 255.0 * 31.0;

      lua[y][x] = RGB_SET((int)r, (int)g, (int)b);
    }
  }
}

#define APPLY(A) \
  for (int y = R.top; y <= R.bottom; y++) for (int x = R.left; x <= R.right; x++) \
  {cell_t* cell = &yeti->map.item[y][x]; A;}

void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  TRect R = RectNormalize(&Selection);
  unsigned tx = Max(ListView->ItemIndex, 0);
  bool sft = Shift.Contains(ssShift);
  bool ctl = Shift.Contains(ssCtrl );
  cell_t* selc = &yeti->map.item[R.top][R.left];


  switch (Key)
  {
    case 221: {APPLY(cell->bot += 64) Key = 1; break;}
    case 219: {APPLY(cell->bot -= 64) Key = 1; break;}

    case 'P': {APPLY(cell->top += 64) Key = 1; break;}
    case 'O': {APPLY(cell->top -= 64) Key = 1; break;}

    case 'L':
    {
      if (ctl)
        APPLY(cell->sw2=((selc->sw2+CELL_SW2_LIGHT1)&CELL_SW2_LIGHT_MASK))
      else
        APPLY(cell->swi ^= CELL_SWI_LIGHT);
      Key = 1;
      break;
    }
    case 'T': {APPLY(cell->swi ^= CELL_SWI_TERRAIN    ) Key = 1; break;}
    case 'C': {APPLY(cell->swi ^= CELL_SWI_CAVE       ) Key = 1; break;}
    case 'H': {APPLY(cell->sw2 ^= CELL_SW2_HALFBRIGHT ) Key = 1; break;}
    case 'Q': {APPLY(cell->sw2 ^= CELL_SW2_OVERPASS   ) Key = 1; break;}    
    case 'K': {APPLY(if (++cell->wwi>8)cell->wwi=0) Key = 1; break;}
    case 'S': {map_smooth_slopes(&yeti->map, R.left, R.top, R.right, R.bottom); Key = 1; break;}

    case '6': {APPLY(cell->tos -= 2;) Key = 1; break;}
    case '7': {APPLY(cell->tos += 2;) Key = 1; break;}
    case '8': {APPLY(cell->bos -= 2;) Key = 1; break;}
    case '9': {APPLY(cell->bos += 2;) Key = 1; break;}

#define WALLTX if(!sft)cell->wtb=tx;if(!ctl)cell->wtt=tx;

    case  45: {if(tc)tx=(tx-1)%TEXTURE_MAX;APPLY(cell->btx=tx);Key=1;break;}
    case  46: {if(tc)tx=(tx+1)%TEXTURE_MAX;APPLY(cell->btx=tx);Key=1;break;}
    case  36: {if(tc)tx=(tx-1)%TEXTURE_MAX;APPLY(WALLTX);Key=1;break;}
    case  35: {if(tc)tx=(tx+1)%TEXTURE_MAX;APPLY(WALLTX);Key=1;break;}
    case  33: {if(tc)tx=(tx-1)%TEXTURE_MAX;APPLY(cell->ttx=tx);Key=1;break;}
    case  34: {if(tc)tx=(tx+1)%TEXTURE_MAX;APPLY(cell->ttx=tx);Key=1;break;}

    case 'F': {APPLY(cell_solid_switch(cell, true )); Key = 1; break;}
    case 'G': {APPLY(cell_solid_switch(cell, false)); Key = 1; break;}

    case '1': {APPLY(cell->swi=cell->swi&~CELL_SWI_LUA_MASK|CELL_SWI_LUA_0);Key=1;break;}
    case '2': {APPLY(cell->swi=cell->swi&~CELL_SWI_LUA_MASK|CELL_SWI_LUA_1);Key=1;break;}
    case '3': {APPLY(cell->swi=cell->swi&~CELL_SWI_LUA_MASK|CELL_SWI_LUA_2);Key=1;break;}
    case '4': {APPLY(cell->swi=cell->swi&~CELL_SWI_LUA_MASK|CELL_SWI_LUA_3);Key=1;break;}

    case 'E':
    {
      String Value = yeti->map.item[R.top][R.left].ent;
      if (InputQuery("", "Enter Entity ID (0..255):", Value))
      {
        int ent = Value.ToIntDef(0);
        APPLY(cell->ent = ent);
        game_spawn_entities(yeti);
        Key = 1;
      }
      break;
    }
    case  VK_LEFT     : {Key = 0; break;}
    case  VK_UP       : {Key = 0; break;}
    case  VK_RIGHT    : {Key = 0; break;}
    case  VK_DOWN     : {Key = 0; break;}
    //case  VK_CONTROL  : {Key = 0; break;}
    case  ' '         : {Key = 0; break;}
    case  'A'         : {Key = 0; break;}
    case  'Z'         : {Key = 0; break;}
    default:
    {
      //ShowMessage(Key);
    }
  }

  if (Key == 1)
  {
    Key = 0;
    map_precalc_data(&yeti->map, yeti->visual.map_lod);
    PaintBox->Repaint();
    FormPreview->TimerTimer(FormPreview->Timer);
    Modified = true;
    ListView->ItemIndex = tx;
    ListView->Selected->MakeVisible(False);
    tc = true;
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)
{
  switch(Key)
  {
    case  VK_LEFT   : {Key = 0; break;}
    case  VK_UP     : {Key = 0; break;}
    case  VK_RIGHT  : {Key = 0; break;}
    case  VK_DOWN   : {Key = 0; break;}
    //case  VK_CONTROL: {Key = 0; break;}
    case  ' '       : {Key = 0; break;}
    case  'A'       : {Key = 0; break;}
    case  'Z'       : {Key = 0; break;}
  }

  String cadena = "";
  cadena= cadena + "Camera Pos: x " + f2i(yeti->camera->x)+" y "+f2i(yeti->camera->y);
  StatusBar->Panels->Items[1]->Text= cadena;

}
//---------------------------------------------------------------------------
void TFormMain::ExportToFile(String AFileName)
{
  yeti_save_map(yeti, &rommap);
  
  TStringList* SL = new TStringList;
  __try
  {
    String LName = ChangeFileExt(ExtractFileName(AFileName), "");

    LName = AnsiReplaceStr(LName, " ", "_");

    SL->Add(Format("YETI_ROM rom_map_t map_%s = {", ARRAYOFCONST((LName))));

    SL->Add(Format("\"%s\",\"%s\",\"%s\",{", ARRAYOFCONST((
      rommap.name,
      rommap.auth,
      rommap.desc))));

    for (int y = 0; y < MAP_HEIGHT; y++)
    {
      SL->Add("{");
      for (int x = 0; x < MAP_WIDTH; x++)
      {
        rom_cell_t* cell = &rommap.cells[y][x];

        SL->Add(Format("{%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d},", ARRAYOFCONST((
          cell->swi,
          cell->ent,
          cell->top,
          cell->wtb,
          cell->sw2,
          cell->bot,
          cell->lit,
          cell->wtt,
          cell->ttx,
          cell->wwi,
          cell->btx,
          cell->tos,
          cell->bos
          ))));
      }
      String S =  SL->Strings[SL->Count - 1];
      SL->Strings[SL->Count - 1] = S.SubString(1, S.Length() - 1);
      SL->Add("},");
    }
    String S =  SL->Strings[SL->Count - 1];
    SL->Strings[SL->Count - 1] = S.SubString(1, S.Length() - 1);
    SL->Add("}};");

    SL->SaveToFile(AFileName);
  }
  __finally
  {
    SL->Free();
  }
}
//---------------------------------------------------------------------------
void TFormMain::SetFileName(String AFileName)
{
  FileName = AFileName;

  if (!AFileName.Length()) AFileName = "untitled";

  Caption = Application->Title + " | " + __DATE__ + " | [" + AFileName + "]";

  Modified = false;
  PaintBox->Repaint();
}
//---------------------------------------------------------------------------
void TFormMain::LoadFromFile(String AFileName)
{
  TFileStream* FS = new TFileStream(AFileName, fmOpenRead);
  try
  {
    FS->ReadBuffer(&rommap, sizeof(rommap));
    game_load_map(yeti, &rommap);
  }
  __finally
  {
    FS->Free();
  }
  SetFileName(AFileName);
}
//---------------------------------------------------------------------------
void TFormMain::SaveToFile(String AFileName)
{
  AFileName = ChangeFileExt(AFileName, ".y3d");

  TFileStream* FS = new TFileStream(AFileName, fmCreate);
  try
  {
    yeti_save_map(yeti, &rommap);
    FS->WriteBuffer(&rommap, sizeof(rommap));
  }
  __finally
  {
    FS->Free();
  }
  ExportToFile(ChangeFileExt(AFileName, ".c"));
  SetFileName(AFileName);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MISaveClick(TObject *Sender)
{
  if (FileExists(FileName) && !FileIsReadOnly(FileName))
  {
    SaveToFile(FileName);
  }
  else
  {
    MISaveAs->Click();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MISaveAsClick(TObject *Sender)
{
  SaveDialog->FileName = FileName;
  if (SaveDialog->Execute()) SaveToFile(SaveDialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ExportCFileClick(TObject *Sender)
{
  if (FileExists(FileName))
  {
    ExportDialog->FileName = ChangeFileExt(FileName, ".c");
  }
  if (ExportDialog->Execute()) ExportToFile(ExportDialog->FileName);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIOpenClick(TObject *Sender)
{
  OpenDialog->FileName = FileName;
  if (OpenDialog->Execute() && CanClose())
  {
    LoadFromFile(OpenDialog->FileName);
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MINewClick(TObject *Sender)
{
  if (CanClose())
  {
    map_init(&yeti->map, yeti->visual.map_lod);
    game_spawn_entities(yeti);
    SetFileName("");
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBNewClick(TObject *Sender)
{
  MINew->Click();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBOpenClick(TObject *Sender)
{
  MIOpen->Click();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBSaveClick(TObject *Sender)
{
  MISave->Click();  
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIAboutClick(TObject *Sender)
{
  ShowMessage(YETI_STR_CAPTION);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = True;
  
  if (Modified)
  {
    switch (Application->MessageBox(
      "Current project has been modified. Do you want to save now?",
      Application->Title.c_str(), MB_YESNOCANCEL))
    {
      case IDYES:
      {
        MISaveAs->Click(); // << This should be Save, but SaveAs is safer.
        CanClose = !Modified;
        break;
      }
      case IDNO:
      {
        CanClose = True;
        break;
      }
      case IDCANCEL:
      {
        CanClose = False;
        break;
      }
    }
  }
}
//---------------------------------------------------------------------------
bool TFormMain::CanClose()
{
  bool LCanClose = True;
  FormCloseQuery(this, LCanClose);
  return LCanClose;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolButton2Click(TObject *Sender)
{
  SetZoom(Zoom + 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolButton3Click(TObject *Sender)
{
  SetZoom(Zoom - 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewChange(TObject *Sender, TListItem *Item, TItemChange Change)
{
  tc = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MISelectAllClick(TObject *Sender)
{
  Selection = Rect(0, 0, MAP_WIDTH - 1, MAP_HEIGHT - 1);
  PaintBox->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ListViewMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
  ListView->Cursor = ListView->GetItemAt(X, Y) ? crHandPoint : crDefault;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MICopyClick(TObject *Sender)
{
  TRect R = RectNormalize(&Selection);

  MapClip.Length = R.Height() + 1;

  for (int i = 0; i < MapClip.Length; i++)
  {
    MapClip[i].Length = R.Width() + 1;
  }

  for (int y = R.top; y <= R.bottom; y++)
  {
    for (int x = R.left; x <= R.right; x++)
    {
      yeti_save_cell(&yeti->map.item[y][x], &MapClip[y - R.top][x - R.left]);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIDeleteClick(TObject *Sender)
{
  TRect R = RectNormalize(&Selection);

  // TODO: Need a range init function.

  map_precalc_data(&yeti->map, yeti->visual.map_lod);

  Modified = True;

  PaintBox->Refresh();  
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MICutClick(TObject *Sender)
{
  MICopy->Click();
  MIDelete->Click();
}
//---------------------------------------------------------------------------
void SurfaceLoadFromIniFile(surface_t* surface, TIniFile* AIniFile, String ASection)
{
  surface_init(surface);
  surface->xsize += AIniFile->ReadInteger(ASection, "xsize", 0);
  surface->ysize += AIniFile->ReadInteger(ASection, "ysize", 0);
  surface->mode   = AIniFile->ReadInteger(ASection, "mode" , surface->mode);
  surface->xpan   = AIniFile->ReadInteger(ASection, "xpan" , surface->xpan);
  surface->ypan   = AIniFile->ReadInteger(ASection, "ypan" , surface->ypan);
}

void __fastcall TFormMain::MIImportTexturesClick(TObject *Sender)
{
  if (OpenPictureDialog->Execute())
  {
    BitmapLoadFromFile(TmpBitmap, OpenPictureDialog->FileName);

    if (TmpBitmap->Width != 64)
    {
      throw Exception("Texture set bitmap must be 64 pixels wide.");
    }
    if (TmpBitmap->Height >= (64 * 128))
    {
      throw Exception("Texture contains more than 128 textures.");
    }
    TmpBitmap->PixelFormat = pf32bit;
    TmpBitmap->PixelFormat = pf8bit;

    memset(textures, 0, sizeof(textures));

    TMaxLogPalette Palette;
    Palette.palVersion = 0x300;
    Palette.palNumEntries = 256;

    GetPaletteEntries(TmpBitmap->Palette, 0, Palette.palNumEntries, Palette.palPalEntry);

    for (int i = Palette.palNumEntries; i--;)
    {
      palette[i][0] = Palette.palPalEntry[i].peRed;
      palette[i][1] = Palette.palPalEntry[i].peGreen;
      palette[i][2] = Palette.palPalEntry[i].peBlue;
    }

    yeti_init_lua(yeti, YETI_GAMMA);

    for (int y = 0; y < TmpBitmap->Height; y++)
    {
      for (int x = 0; x < TmpBitmap->Width; x++)
      {
        textures[y >> 6][63 - (y & 63)][x & 63] = ((Byte*)TmpBitmap->ScanLine[y])[x];
      }
    }
    TIniFile* LIniFile = new TIniFile(ChangeFileExt(OpenPictureDialog->FileName, ".ini"));
    try
    {
      for (int i = 0; i < yeti->surfaces.length; i++)
      {
        SurfaceLoadFromIniFile(&yeti->surfaces.item[i], LIniFile, i);
      }
    }
    catch (...)
    {
      LIniFile->Free();
    }
    UpdateTextureList();
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
  PanelCamera->Color = PanelCamera->Color != clWhite ?  clWhite : clRed;
}
//---------------------------------------------------------------------------



int yeti_save_file(void *buffer, int size, char* filename)
{
  FILE* fp = fopen(filename, "wb");

  if (fp)
  {
    int n;
       n = fwrite(buffer, 1, size, fp);

    fclose(fp);
    return n;
  }

  return 0;
}


void __fastcall TFormMain::MIExportTexturesPaletteClick(TObject *Sender)
{
 if (SaveDialogText->Execute()) {
   char ruta[512];
   //texturas
   strcpy(ruta, SaveDialogText->FileName.c_str());
   yeti_save_file(textures,sizeof(textures), ruta);

   //paleta
   String rutapal= ChangeFileExt(SaveDialogText->FileName, ".pal");
   strcpy(ruta, rutapal.c_str());
   yeti_save_file(palette,sizeof(palette), ruta);
 }

}
//---------------------------------------------------------------------------

