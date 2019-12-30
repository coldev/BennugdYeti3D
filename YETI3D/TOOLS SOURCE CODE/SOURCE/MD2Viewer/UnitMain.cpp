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
  PBBackDrop->ControlStyle = PBBackDrop->ControlStyle << csOpaque;
  PBScene->ControlStyle = PBScene->ControlStyle << csOpaque;

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

  yeti_init(&yeti,
    framebuffer.pixels[0],
    framebuffer.pixels[0],
    textures, palette);/*,
    lua0, lua1, lua2, lua3);*/

  game_load_map(&yeti, &map_e3m1);
  game_goto(&yeti.game, GAME_MODE_PLAY);
  //yeti_init_map(&yeti);
  //yeti_calc_lights(&yeti);

  yeti.camera->x = i2fdiv2(MAP_WIDTH ) + FIXHALF;
  yeti.camera->y = i2fdiv2(MAP_HEIGHT) + FIXHALF;

  //yeti_light(&yeti, yeti.camera->x, yeti.camera->y);

  entity = yeti_entity(&yeti, yeti.camera->x, yeti.camera->y, 0);
  entity->visual.data   = 0;
  entity->visual.width  = 8;
  entity->visual.height = 10;
  entity->t = DEG_180;
  entity->radius = 0;
  entity->visual.ondraw = yeti_model_draw;

  LoadFromFile("tris.md2", "skin.jpg");

  yeti.camera->y -= i2f(2);
  
  SetZoom(2);
}
//---------------------------------------------------------------------------
void TFormMain::SetZoom(int AZoom)
{
  Zoom = EnsureRange(AZoom, 1, 10);
  ScrollBox->OnResize(ScrollBox);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PBScenePaint(TObject *Sender)
{
  PBScene->Canvas->StretchDraw(PBScene->ClientRect, Bitmap);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ScrollBoxResize(TObject *Sender)
{
  int LWidth = Bitmap->Width * Zoom;
  int LHeight = Bitmap->Height * Zoom;

  PanelScene->SetBounds(
    Max((ScrollBox->ClientWidth  - LWidth ) / 2, 0) - ScrollBox->HorzScrollBar->Position,
    Max((ScrollBox->ClientHeight - LHeight) / 2, 0) - ScrollBox->VertScrollBar->Position,
    LWidth, LHeight);
}

//---------------------------------------------------------------------------
void ModelConvert(TMemoryStream* Dst, TMemoryStream* Src, Graphics::TBitmap* ASkin)
{
#define WRITE(A) {short n = (A); Dst->Write(&n, sizeof(n));}

  model_t* model = (model_t*) Src->Memory;
  triangle_t* triangles = (triangle_t*)((int)model + model->offsetTriangles);
  textureCoordinate_t* texcoords = (textureCoordinate_t*)((int)model + model->offsetTexCoords);

  Dst->Clear();

  WRITE(7);
  WRITE(model->numFrames  );
  WRITE(model->numVertices);
  WRITE(model->numTexCoords);
  WRITE(model->numTriangles);
  WRITE(model->numVertices * 3);
  WRITE(1);

  for (int fid = 0; fid < model->numFrames; fid++)
  {
    frame_t* f = (frame_t*)((int)model + model->offsetFrames + model->frameSize * fid);

    for (int vid = 0; vid < model->numVertices; vid++)
    {
      WRITE(-(f->vertices[vid].vertex[1] * f->scale[1] + f->translate[1]) * 16);
      WRITE( (f->vertices[vid].vertex[2] * f->scale[2] + f->translate[2]) * 16 + 256);
      WRITE( (f->vertices[vid].vertex[0] * f->scale[0] + f->translate[0]) * 16);
    }
  }

  for (int i = 0; i < model->numTexCoords; i++)
  {
    int s = texcoords[i].s * i2f(YETI_SKIN_WIDTH ) / model->skinWidth;
    int t = texcoords[i].t * i2f(YETI_SKIN_HEIGHT) / model->skinHeight;

    //s = CLAMP(s, 0, i2f(YETI_SKIN_WIDTH ) - 1);
    t = CLAMP(t, 0, i2f(YETI_SKIN_HEIGHT) - 1);

    WRITE(s); WRITE(t);
  }
  for (int tid = 0; tid < model->numTriangles; tid++)
  {
    WRITE(triangles[tid].vertexIndices[0]);
    WRITE(triangles[tid].vertexIndices[1]);
    WRITE(triangles[tid].vertexIndices[2]);
    WRITE(triangles[tid].textureIndices[0]);
    WRITE(triangles[tid].textureIndices[1]);
    WRITE(triangles[tid].textureIndices[2]);
  }

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
  MD2Model->LoadFromFile(AFileName);

  model_t* model = (model_t*) MD2Model->Memory;

  Edit1->Text = String((char*)&model->magic, 4);
  Edit2->Text = model->version;
  Edit3->Text = model->numFrames;
  Edit4->Text = model->numTriangles;
  Edit5->Text = model->numVertices;
  Edit6->Text = model->numTexCoords;
  Edit7->Text = model->numSkins;

  TBFrame->Max = model->numFrames - 1;
}

void TFormMain::LoadSkin(String AFileName)
{
  BitmapLoadFromFile(BMSkin, AFileName);
  BMSkin->PixelFormat = pf32bit;
  BitmapResize(BMSkin, YETI_SKIN_WIDTH, YETI_SKIN_HEIGHT);
  PBSkin->Refresh();
}

void TFormMain::LoadFromFile(String AModelName, String ASkinName)
{
  LoadModel(AModelName);
  LoadSkin(ASkinName);

  entity->visual.data = NULL;

  ModelConvert(YetiModel, MD2Model, BMSkin);

  model_t* model = (model_t*) MD2Model->Memory;

  if (model->numTriangles >= YETI_MODEL_TRANGLE_MAX)
  {
    throw Exception("The MD2 contains too many triangles to render.");
  }
  if (model->numVertices >= YETI_MODEL_VERTEX_MAX)
  {
    throw Exception("The MD2 contains too many vertices to render.");
  }

  entity->visual.data = YetiModel->Memory;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIOpenClick(TObject *Sender)
{
  OpenDialog->FileName = "";
  OpenDialog->Title = "Select MD2 File...";
  OpenDialog->Filter = "MD2 Files (*.md2)|*.md2";

  if (OpenDialog->Execute())
  {
    String ModelName = OpenDialog->FileName;

    OpenDialog->FileName = "";
    OpenDialog->Title = "Select Skin Bitmap...";    
    OpenDialog->Filter = "Bitmap Files (*.bmp)|*.bmp;*.jpg;*.jpeg";

    if (OpenDialog->Execute())
    {
      LoadFromFile(ModelName, OpenDialog->FileName);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TimerTimer(TObject *Sender)
{
  if (yeti.keyboard.tick & 1)
  {
    if (CBAnimate->Checked)
    {
      if (TBFrame->Position == TBFrame->Max)
        TBFrame->Position = TBFrame->Min;
      else
        TBFrame->Position = TBFrame->Position + 1;
    }
  }
  keyboard_update(&yeti.keyboard);
  game_loop(&yeti);

  //entity->z = yeti.camera->z;

  BitmapDrawViewport(Bitmap, &yeti.viewport);

  PBScene->OnPaint(PBScene);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBZoomInClick(TObject *Sender)
{
  SetZoom(Zoom + 1);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::TBZoomOutClick(TObject *Sender)
{
  SetZoom(Zoom - 1);
}
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
void __fastcall TFormMain::PBBackDropPaint(TObject *Sender)
{
  for (int Y = 0; Y < PBBackDrop->ClientHeight; Y += BMBackDrop->Height)
  {
    for (int X = 0; X < PBBackDrop->ClientWidth; X += BMBackDrop->Width)
    {
      PBBackDrop->Canvas->Draw(X, Y, BMBackDrop);
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExitClick(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::MIExportBinClick(TObject *Sender)
{
  SaveDialog->FileName = "";
  SaveDialog->Title = "Export BIN File...";
  SaveDialog->DefaultExt = "bin";
  SaveDialog->Filter = "Bin Files (*.bin)|*.bin";

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
void __fastcall TFormMain::TBFrameChange(TObject *Sender)
{
  entity->animation.pos = i2f(TBFrame->Position);

  Edit8->Text = TBFrame->Position;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PBSceneMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  MDP = Point(X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::PBSceneMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
  if (PBScene->ControlState.Contains(csLButtonDown))
  {
    entity->t += i2f(MDP.x - X) * 3;
    MDP = Point(X, Y);
  }
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::Edit9Change(TObject *Sender)
{
 entity->z =  StrToInt(Edit9->Text ) ;

}
//---------------------------------------------------------------------------

