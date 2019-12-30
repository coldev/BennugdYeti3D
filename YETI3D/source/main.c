//#include "gp2x.h"
#ifndef GP2X
#include "y3d_keyboard.h"
#endif
#include "y3d_yeti.h"
#include "maps.h"
#include "game.h"
#include "y3d_viewport.h"
#include "y3d_pixel.h"
#include "y3d_draw.h"


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
//#include <SDL/SDL.h>
//#include "flush_uppermem_cache.h"   // Squidgehack

#define YETI_GAMMA (2.1)



#include <bgddl.h>
#include "dlvaracc.h"
#include "libgrbase.h"
#include "libvideo.h"


typedef u16 rgb555_t;
typedef struct
{
  rgb555_t pixels[YETI_VIEWPORT_Y2_MAX][YETI_VIEWPORT_X2_MAX];
} framebuffer_t;

framebuffer_t framebuffer; //gives us somewhere to draw our stuff.
yeti_t yeti;
int oldTick = 0;		// Used to help limit the framerate.
int done = 0;



void Yeti_Cleanup();

static pixel_converter_t pixel_converter;
rom_map_t yetiMap;
/*
  int YETI_FRAMEBUFFER_WIDTH = 800;
  int YETI_FRAMEBUFFER_HEIGHT = 600;
  int YETI_VIEWPORT_X2 =800;
  int YETI_VIEWPORT_Y2 =600;
*/



#ifdef __WIN32__

  int YETI_RAY_WIDTH=50;
  int YETI_RAY_MAX=  512;
  int YETI_CELL_MAX= 512;
#else

  int YETI_RAY_WIDTH=350;
  int YETI_RAY_MAX=  64;
  int YETI_CELL_MAX= 512;
#endif

//-------------------------------
//bennu vars




GRAPH *pantalla;

int iniciado_3d=0;

#define MAX_MODELOS 100
#define MAX_ARCHIVOS 50

entity_t *entidades[MAX_MODELOS];
void * archivos[MAX_ARCHIVOS];


int modelocont=0,archivocont=0;

 rom_map_t* mapa = 0;
 texture_t *texturas = 0;
 palette_t  paleta ;


//------------------------

/* Definicion de variables globales (usada en tiempo de compilacion) */

#define YETI_AUTO_RENDER       0
#define YETI_BULLET_WIDTH      1
#define YETI_BULLET_HEIGHT     2
#define YETI_BULLET_DRAWMODE   3
#define YETI_BULLET_Z          4
#define YETI_BULLET_RADIUS     5
#define YETI_SCREEN_WIDTH      6
#define YETI_SCREEN_HEIGHT     7




DLLEXPORT char *  __bgdexport(yeti3d,globals_def)=
    "INT YETI_AUTO_RENDER = 1; \n"
    "INT YETI_BULLET_WIDTH = 64; \n"
    "INT YETI_BULLET_HEIGHT = 64; \n"
    "INT YETI_BULLET_DRAWMODE = 0; \n"
    "INT YETI_BULLET_Z = 0; \n"
    "INT YETI_BULLET_RADIUS = 10; \n"
    "INT YETI_SCREEN_WIDTH = 640 ; \n"
    "INT YETI_SCREEN_HEIGHT = 480; \n"
    ;





/* ----------------------------------------------------------------- */
/* Son las variables que se desea acceder.                           */
/* El interprete completa esta estructura, si la variable existe.    */
/* (usada en tiempo de ejecucion)
      */
DLLEXPORT DLVARFIXUP  __bgdexport(yeti3d,globals_fixup)[] = {
    /* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */
       { "YETI_AUTO_RENDER" , NULL, -1, -1 },
       { "YETI_BULLET_WIDTH" , NULL, -1, -1 },
       { "YETI_BULLET_HEIGHT" , NULL, -1, -1 },
       { "YETI_BULLET_DRAWMODE" , NULL, -1, -1 },
       { "YETI_BULLET_Z" , NULL, -1, -1 },
       { "YETI_BULLET_RADIUS" , NULL, -1, -1 },
       { "YETI_SCREEN_WIDTH" , NULL, -1, -1 },
       { "YETI_SCREEN_HEIGHT" , NULL, -1, -1 },

       { NULL, NULL, -1, -1 }
                                };







//Hack  by ColDev

#define MIN(A,B) ((A)<(B)?(A):(B))
//Hack  by ColDev
int lua_rgb (int r, int g, int b)
{
int r1 = (MIN (r, 255) * 31) / 255;
int g1 = (MIN (g, 255) * 31) / 255;
int b1 = (MIN (b, 255) * 31) / 255;

return (r1 << 10) + (g1 << 5) + b1;
}


//---------------------------------------------------
//	SPRITES
//---------------------------------------------------
typedef rgb555_t* sprite_t;

typedef struct Mic
{
    uint8_t rojo;
    uint8_t verde;
    uint8_t azul;

} mic ;

/*
//u8* SpriteGlobaldata;
int NSPRITES;
sprite_t* sprites;
*/

#include "y3d_image.h"

static int fx_CargarSpritesFenix(INSTANCE * my, int * params)
{
	GRAPH * map  ;
	int x,y,   pixel, tamano;
    sprite_t* sprtab;

    map = bitmap_get (params[0],  params[1]) ;

    if (!map) return 0;
    if (map->format->depth != 8 ) return 0;

//------------------------------------
    tamano=  map->width * map->height;
    tamano= (sizeof(u16)*tamano) + (sizeof(u16)*2);
    u16 *pixeldata= (u16 *) malloc( tamano);
    memset(pixeldata, 0, tamano);


printf("Fenix Sprite: malloc %d \n",tamano);

//------------------------------------
    sprtab = (sprite_t*)malloc( sizeof(sprite_t) );
    memset(sprtab, 0, sizeof(sprite_t));

//------------------------------------
    mic colors[256];
    PALETTE * pal1;
    pal1=map->format->palette;
    pal_get(pal1, 0, 256, ( uint8_t * ) colors );

//------------------------------------
    IMAGE_WIDTH(pixeldata) = map->width;
    IMAGE_HEIGHT(pixeldata)= map->height;

    for   (x=0;  x < map->width;  x++)
	 for (y=0; y < map->height; y++)
	 {
	    pixel= gr_get_pixel (map, x, y);
	    pixel= lua_rgb(colors[pixel].rojo,colors[pixel].verde,colors[pixel].azul);

        IMAGE_PIXEL(pixeldata,x,y)= pixel;
	 }


	sprtab[0]= (sprite_t) pixeldata ;

	pixel_convert_image(sprtab[0], 31 << 10, 31 << 5, 31 << 0);//new pixel data in Pro version


    printf("Fenix Sprite:  w %d h %d \n",  IMAGE_WIDTH(sprtab[0]), IMAGE_HEIGHT(sprtab[0]));

  return &sprtab[0];
}


int Load_Sprites(char  *sTextura)
{
	int size,i, nbspr, sprsize;
	u8* data ;
	sprite_t* sprtab;
	u32 start, end, j;


	//UnloadSprites();

	data= yeti_load_file(sTextura,&size);
	if (!data) return 0;

	nbspr = *((int*)data);
	sprsize = *((int*)(data+4));

	//NSPRITES= nbspr;

	// check file validity
	if (sprsize+8+nbspr*4!=size)	return 0;

	//SpriteGlobaldata= data;

	// get sprites
	sprtab = (sprite_t*)malloc(sizeof(sprite_t)*nbspr);

	for (i=0;i<nbspr;i++)
	{
		start = 8+nbspr*4+((u32*)(data+8))[i];
		sprtab[i] = (sprite_t)(data+start);

#ifdef __GP32__
		end = i+1==nbspr ? size : 8+nbspr*4+((u32*)(data+8))[i+1];
		for (j=start+2;j<end;++j)
			sprtab[i][j] = sprtab[i][j]<<1;
#endif
	}

	//sprites= sprtab;

  for (i=0;i<nbspr;i++)
    pixel_convert_image(sprtab[i], 31 << 10, 31 << 5, 31 << 0);//new pixel data in Pro version


	//cuenta abiertos
  if (data) {
     archivos[archivocont] = data;
     archivocont++;
  }

  printf("Yeti Sprites: %d, w %d h %d \n",nbspr,  IMAGE_WIDTH(sprtab[0]), IMAGE_HEIGHT(sprtab[0]));

  return &sprtab[0];
}





//---------------------------------------------------
//	TEXTURES
//---------------------------------------------------



//colors fixed by PRG ... thanks

static int fx_CargarTexturaFenix(INSTANCE * my, int * params)
{
	GRAPH * map  ;
	int x,y,   pixel,  grafico;

    map = bitmap_get (params[0],  params[1]) ;
    if (!map) return 0;
    if (map->format->depth != 8 ) return 0;
    if (map->width != 64 ) return 0;
    if (map->height > 64 * TEXTURE_MAX ) return 0;

    mic colors[256];
    PALETTE * pal1;
    pal1=map->format->palette;
    pal_get(pal1, 0, 256, ( uint8_t * ) colors );

    if (texturas) free (texturas);//-------------------free

    int tamano= (TEXTURE_HEIGHT*TEXTURE_WIDTH)* ( TEXTURE_MAX );

    texturas=  malloc(tamano);

	 memset(texturas, 0, tamano);


    for   (x=0;  x < map->width;  x++)
	   for (y=0; y < map->height; y++)
		{
           pixel= gr_get_pixel (map, x, y);

		   texturas[y >> 6][63 - (y & 63)][x & 63] = pixel;

           paleta[pixel][0]=colors[pixel].rojo;  //load fenix palette
		   paleta[pixel][1]=colors[pixel].verde;
		   paleta[pixel][2]=colors[pixel].azul;
		}

	printf("Bennu Texturas. \n" );


   return 1;
}










typedef struct bennucell_t
{
  int  swi;  // Cell flags.
  int  ent;  // Entity Id. Used for setting start locations for entities.
  int top;  // 8:8 fixed point cell top value.
  int bot;  // 8:8 fixed point cell bot value.
  int eos;  // Entity offset.
  int hos;  // Height offset.
  int  tos;  // Top offset. Used for caves.
  int  bos;  // Bottom offset. Used for terrains.
  int bots[4];
  int tops[4];

  int  wtb;  // Cell wall bot texture id.
  int  sw2;
  int lit;  // Cell light setting.
  int  wtt;  // Cell wall top texture id.
  int  ttx;  // Cell top texture id.
  int  btx;  // Cell bot texture id.
  int  wwi;  // Wall warp index.

  int  xlod;
  int  ylod;
  int mark;


  //internal use only
  int pcell;

} bennucell_t;


#define ENTIDAD_CELDA(X,Y) (&yeti.map.item[Y][X])


void celdacopiarayeti( cell_t* cell, bennucell_t *b)
{

   cell->swi= b->swi;
   cell->ent= b->ent;
   cell->top= b->top;
   cell->bot= b->bot;
   cell->eos= b->eos;
   cell->hos= b->hos;
   cell->tos= b->tos;
   cell->bos= b->bos;

   cell->bots[0]= b->bots[0];
   cell->bots[1]= b->bots[1];
   cell->bots[2]= b->bots[2];
   cell->bots[3]= b->bots[3];

   cell->tops[0]= b->tops[0];
   cell->tops[1]= b->tops[1];
   cell->tops[2]= b->tops[2];
   cell->tops[3]= b->tops[3];

   cell->wtb= b->wtb;
   cell->sw2= b->sw2;
   cell->lit= b->lit;
   cell->wtt= b->wtt;
   cell->ttx= b->ttx;
   cell->btx= b->btx;
   cell->wwi= b->wwi;
   cell->xlod= b->xlod;
   cell->ylod= b->ylod;
   cell->mark= b->mark;

    map_precalc_data(&yeti.map, yeti.visual.map_lod);

}



void celdacopiarabennu(  bennucell_t *b, cell_t* cell)
{
   b->swi= cell->swi;
   b->ent= cell->ent;
   b->top= cell->top;
   b->bot= cell->bot;
   b->eos= cell->eos;
   b->hos= cell->hos;
   b->tos= cell->tos;
   b->bos= cell->bos;

   b->bots[0]= cell->bots[0];
   b->bots[1]= cell->bots[1];
   b->bots[2]= cell->bots[2];
   b->bots[3]= cell->bots[3];

   b->tops[0]= cell->tops[0];
   b->tops[1]= cell->tops[1];
   b->tops[2]= cell->tops[2];
   b->tops[3]= cell->tops[3];

   b->wtb= cell->wtb;
   b->sw2= cell->sw2;
   b->lit= cell->lit;
   b->wtt= cell->wtt;
   b->ttx= cell->ttx;
   b->btx= cell->btx;
   b->wwi= cell->wwi;
   b->xlod= cell->xlod;
   b->ylod= cell->ylod;
   b->mark= cell->mark;

   b->pcell=(int) cell;
}



static int fx_obtenercelda3ddeID(INSTANCE * my, int * params)
{
  cell_t* cell=  (cell_t*) params[0];
  bennucell_t *b= (bennucell_t *) params[1] ;

  celdacopiarabennu(b,cell  );
}


static int fx_obtenercelda3d(INSTANCE * my, int * params)
{
  cell_t* cell= ENTIDAD_CELDA(params[0],params[1]);
  bennucell_t *b= (bennucell_t *) params[2] ;

  celdacopiarabennu(b,cell  );
}


static int fx_modificarcelda3d(INSTANCE * my, int * params)
{
  bennucell_t *b= (bennucell_t *) params[0] ;

  celdacopiarayeti((cell_t*) b->pcell,  b );

}




void Entidadcopiarayeti(entity_t* e,bennu_entity_t *b)
{
   e->x= b->x;
   e->xx= b->xx;

   e->y= b->y;
   e->yy= b->yy;

   e->z= b->z;
   e->zz= b->zz;

   e->p= b->p;
   e->pp= b->pp;

   e->r= b->r;
   e->rr= b->rr;

   e->t= b->t;
   e->tt= b->tt;

   e->rx= b->rx;
   e->ry= b->ry;
   e->rz= b->rz;

  e->emitlight=     b->emitlight;
  e->castshadow=    b->castshadow;
  e->nocollision=   b->nocollision;
  e->alwaysvisible= b->alwaysvisible;
  e->alwaysontop=   b->alwaysontop;

  e->DetectedCollisionWithEntity= b->DetectedCollisionWithEntity;//Ent-Ent Cols
  e->DetectedCollisionWithEntityId= b->DetectedCollisionWithEntityId;//Ent-Ent Cols
  e->DetectedCollisionWithWall=   b->DetectedCollisionWithWall; // Cells

  e->Msg= b->Msg;
  e->FixZ= b->FixZ;
  e->nocollisionwall= b->nocollisionwall;
  e->ScaleModel= b->ScaleModel;
}



void Entidadcopiarabennu(bennu_entity_t *b, entity_t* e)
{
   b->x= e->x ;
   b->xx= e->xx ;

   b->y= e->y ;
   b->yy= e->yy;

    b->z= e->z;
   b->zz= e->zz ;

   b->p= e->p;
   b->pp= e->pp;

   b->r= e->r ;
   b->rr= e->rr ;

   b->t= e->t ;
   b->tt= e->tt ;

   b->rx= e->rx ;
   b->ry= e->ry;
   b->rz= e->rz;

  b->emitlight=     e->emitlight;
  b->castshadow=    e->castshadow;
  b->nocollision=   e->nocollision;
  b->alwaysvisible= e->alwaysvisible;
  b->alwaysontop=   e->alwaysontop;

  b->DetectedCollisionWithEntity= e->DetectedCollisionWithEntity;//Ent-Ent Cols
  b->DetectedCollisionWithEntityId= e->DetectedCollisionWithEntityId;//Ent-Ent Cols
  b->DetectedCollisionWithWall=   e->DetectedCollisionWithWall; // Cells


  b->Msg= e->Msg;
  b->FixZ= e->FixZ;
  b->nocollisionwall= e->nocollisionwall;
  b->ScaleModel= e->ScaleModel;
}


static int fx_loadmap(INSTANCE * my, int * params)
{
   const char *rutamap = string_get(params[0]) ;
   int size;
   int i, x, y;

   if (mapa) free(mapa);

   mapa = yeti_load_file(rutamap,&size);
   string_discard (params[0]) ;

   if (!mapa) return 0;


   rom_map_t* src= mapa;   //low loading

  for (y = 0; y < MAP_HEIGHT; y++)
  {
    for (x = 0; x < MAP_WIDTH; x++)
    {
      yeti_load_cell(&yeti.map.item[y][x], &src->cells[y][x]);
    }
  }
  map_precalc_data(&yeti.map, yeti.visual.map_lod);


   return 1;
}


static int fx_savemap(INSTANCE * my, int * params)
{
   const char *rutamap = string_get(params[0]) ;
   int size;
   rom_map_t src;

   yeti_save_map(&yeti, &src);

   size = yeti_save_file(&src, sizeof(rom_map_t), rutamap);
   string_discard (params[0]) ;

   return size;
}





//#include "pixelbennu.c"


static void fx_Render(int OnlyRender)
{

  if (!OnlyRender)
  {
   yeti_tick(&yeti);
   keyboard_tick(&yeti.keyboard);
  }

  yeti_draw(&yeti);

  if (pantalla)
  {
    pixel_buffer_draw(yeti.viewport.back, (rgb555_t*)pantalla->data, pantalla->pitch, &pixel_converter);
   // funcionrender3d();
    pantalla->modified=1;
  }

}



static int fx_setrendergraph(INSTANCE * my, int * params)
{
 pantalla = bitmap_get (params[0], params[1]) ;
 if (! pantalla ) return 0;
/*
    YETI_FRAMEBUFFER_WIDTH  = pantalla->width ;
    YETI_FRAMEBUFFER_HEIGHT =pantalla->height ;
    YETI_VIEWPORT_X2 = pantalla->width ;
    YETI_VIEWPORT_Y2 =pantalla->height ;
/*
      switch (YETI_FRAMEBUFFER_WIDTH)
      {
        case 800:

          break;
        case 640:

          break;
        case 320:
          funcionrender3d= render3d_320;
          break;
        case 160:

          break;
      }
*/
 return 1;
}



static int fx_settexture(INSTANCE * my, int * params)
{
    const char *rutatext = string_get(params[0]) ;
    const char *rutapal = string_get(params[1]) ;
    int size;

    if (texturas) free( texturas);

      texturas =      yeti_load_file(rutatext,&size);
      void *ppaleta = yeti_load_file(rutapal,&size);

      if (ppaleta)
      {
        memcpy((void *)paleta, ppaleta, sizeof(paleta));
      }

    string_discard (params[0]) ;
    string_discard (params[1]) ;

    if (!texturas) return 0;
    if (!ppaleta) return 0;

    free( ppaleta);

     printf("Yeti Textures size %d . \n",size);

    return 1;
}


static int fx_setrendereffect(INSTANCE * my, int * params)
{
   pixel_converter_init(&pixel_converter, 31 << 11, 31 << 6, 31 << 0, params[0]);

}


static int fx_setrendereffect_rgb(INSTANCE * my, int * params)
{
   pixel_converter_init(&pixel_converter, params[0], params[1], params[2], params[3]);

}


static int fx_setsky(INSTANCE * my, int * params)
{

  yeti.skyimage =((sprite_t*) params[0])[0];

  return 1;
}


#include "y3d_engine.h"

void bala_tick(yeti_t* yeti, entity_t* e);


void colision_hit(yeti_t* yeti, entity_t* e1, entity_t* e2)
{

   // printf("collision hay  \n" );
   if (e1->CollisionId == e2->CollisionId) return;

   if ( e1->CollisionOwner ==  e2 ||
          e2->CollisionOwner ==  e1) return;
// printf("paso coolll  \n" );


   if (e1->CollisionWithId & e2->CollisionId)
   {
     e1->DetectedCollisionWithEntity= e2;//Ent-Ent Cols
     e1->DetectedCollisionWithEntityId= e2->CollisionId;//guarda con quien colisiono
    // printf("coll e1 %d e2 %d  \n",e1->CollisionId,e2->CollisionId );
   }

   if (e2->CollisionWithId & e1->CollisionId)
   {
     e2->DetectedCollisionWithEntity= e1;//Ent-Ent Cols
     e2->DetectedCollisionWithEntityId= e1->CollisionId;//guarda con quien colisiono
     //printf("coll e2 %d e1 %d  \n",e2->CollisionId,e1->CollisionId );
   }

   if (e1->ontick == bala_tick )//es una bala
   {
     //printf("bullet e1 %d e2 %d  \n",e1->CollisionId,e2->CollisionId );

     entity_destroy(e1);
   }
}


void colision_wall(yeti_t* yeti, entity_t* e, cell_t* c)
{
  // printf("collision wall  \n");
   e->DetectedCollisionWithWall= c;//



   if (e->ontick == bala_tick)//es una bala
   {
       entity_destroy(e);
   }
}



void jugador_tick(yeti_t* yeti, entity_t* e)
{

    animation_tick(&e->animation);

    if (e->animation.mode == 1 && !e->animation.nloops)
    {
      animation_init(&e->animation, 0, 0, 8, 192, LOOP_FOREVER);
    }

//    e->visual.data      = NULL;// yeti->camera == yeti->player ? model_rocket : NULL;
    e->visual.altpitch  = TRUE;
    e->visual.ondraw    = yeti_model_draw;

    //entity_default(yeti, e, 0, FALSE, 60);
}


void jugador_init(entity_t* e)
{
  e->radius =128;
  e->nocollision=0;
  e->ontick = jugador_tick;

  animation_init(&e->animation, 0, 0, 8, 192, LOOP_FOREVER);
}


static int fx_Iniciar(INSTANCE * my, int * params)
{
    const char *rutamap = string_get(params[1]) ;
    int size;

    mapa = yeti_load_file(rutamap,&size);
    string_discard (params[1]) ;

    if (! pantalla ) return 0;

    if (!mapa) return 0;
    if (!texturas) return 0;
    if (!paleta) return 0;


    iniciado_3d= 1;


    //framebuffer= malloc( sizeof(pixel_buffer_800));

    yeti_init(&yeti,&framebuffer, &framebuffer, texturas, paleta);
    yeti_init_lua(&yeti, YETI_GAMMA);


    game_load_map(&yeti, mapa);
    game_goto(&yeti.game, GAME_MODE_PLAY);
    jugador_init(yeti.player);




  //  pixel_converter_init(&pixel_converter, 31 << 11, 31 << 6, 31 << 0, PIXEL_FILTER_NORMAL);
  pixel_converter_init(&pixel_converter, 31 << 11, 31 << 6, 31 << 1, PIXEL_FILTER_NORMAL);


   yeti.player->bennu_e= (bennu_entity_t *)  params[0];

   Entidadcopiarabennu(yeti.player->bennu_e,  yeti.player);

    yeti.player->bennu_e->yetient= yeti.player;//yeti ref

   //memcpy(yeti.player->bennu_e,(bennu_entity_t *) yeti.player, sizeof(bennu_entity_t)); //inicializa bennu entity




   printf("...YETI3D module Init - OK ...  \n");

	return 1;
}

// Lineus - handy way to clean up... called at exit of program
// place all cleanup code here - SDL_Quit(), etc...
static int fx_Terminar(INSTANCE * my, int * params)
{
    iniciado_3d= 0;

    if (mapa) free(mapa);
    if (texturas) free(texturas);
    //if (paleta) free(paleta);
    //if (framebuffer) free(framebuffer);

 mapa = 0;
 texturas = 0;
 //paleta =0;
 //framebuffer=0;

 int i;
 for (i=0;i< archivocont ;i++) {
        if (archivos[i]) free(archivos[i]) ;
      }

 modelocont=0;
 archivocont=0;

 printf("...YETI3D module Finished - OK ...  \n");
}



static int fx_loadfile(INSTANCE * my, int * params)
{
  const char *ruta = string_get(params[0]) ;
  int size;

  void *i =  yeti_load_file(ruta,&size);
  string_discard (params[0]) ;

  if (i) {
     archivos[archivocont] = i;
     archivocont++;
  }

  return  i;
}



static int fx_fixsin(INSTANCE * my, int * params)
{
  return  fixsin(params[0]) ;
}


static int fx_fixcos(INSTANCE * my, int * params)
{
  return  fixcos(params[0]) ;
}


static int fx_fixangle(INSTANCE * my, int * params)
{
  return  fixangle(params[0],params[1]) ;
}


static int fx_lupin_arctan2(INSTANCE * my, int * params)
{
  return  lupin_arctan2(params[0],params[1]) ;
}


static int fx_isqrt(INSTANCE * my, int * params)
{
  return  isqrt(params[0]) ;
}



static int fx_cellbot(INSTANCE * my, int * params)
{
  bennucell_t *bc=   params[0];
  entity_t *e =  params[1];

  return  cell_bot((cell_t *)bc->pcell,   e) ;
}


static int fx_celltop(INSTANCE * my, int * params)
{
  bennucell_t *bc=   params[0];
  entity_t *e =  params[1];

  return  cell_top((cell_t *)bc->pcell,   e) ;
}


static int fx_ansic_friction(INSTANCE * my, int * params)
{
  return  ansic_friction(params[0], params[1]) ;
}



//3d models


static int fx_modelanimation(INSTANCE * my, int * params)
{
  bennu_entity_t* be =(bennu_entity_t* ) params[0];
  entity_t *e= (entity_t *)be->yetient;

  animation_init(&e->animation, 1, params[1], params[2], params[3], params[4]);

  return   0;
}



void modelo_stand_tick(yeti_t* yeti, entity_t* e)
{
  animation_tick(&e->animation);
}



void modelo_stand_init(entity_t* e)
{
  e->visual.ondraw = yeti_model_draw;
  e->visual.mode= DRAW_MODE_COPY;
 /* e->visual.width  = i2f(2);
  e->visual.height = i2f(2);
*/

  e->radius = 128;
  e->nocollision=0;

  e->ontick = modelo_stand_tick;

  animation_init(&e->animation, 0,   0, 1, 80, LOOP_FOREVER);//fix to models with 1 frame

}


static int fx_entity_collisionEnable(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];
  entity_t* e=  (entity_t*) be1->yetient;

  e->DetectedCollisionWithEntity=  e->DetectedCollisionWithWall= 0;

  if ( params[1])
  {
    e->onwall = colision_wall;
    e->onhit  = colision_hit;


    e->CollisionId= params[2]; //collision flags
    e->CollisionWithId= 0;
    e->CollisionWithId |= params[3];
  }
  else
  {
    e->onwall =  e->onhit  = 0;
  }

  return  1 ;
}



static int fx_entity_msg(INSTANCE * my, int * params)
{
  entity_t* e= (entity_t*)   params[0];
  e->Msg= params[1] ;

  bennu_entity_t* b= e->bennu_e;
  b->Msg= params[1] ;


  return  1 ;
}




static int fx_newmodel(INSTANCE * my, int * params)
{
    if (yeti.entities.length >=  (MAX_MODELOS - 1) ) return 0;


    int modelo  =params[0] ;
//    if (!modelo ) return 0;

    bennu_entity_t *pos= (bennu_entity_t *)params[1];

    entity_t *e= yeti_entity(&yeti,  pos->x,  pos->y,  pos->z);
    e->visual.data = (void *) modelo;

    modelo_stand_init    (e);

    //memcpy(pos,(bennu_entity_t *) e, sizeof(bennu_entity_t)); //inicializa bennu entity
    Entidadcopiarabennu(pos, e);

    e->bennu_e= pos;
    pos->yetient= e; //yeti ref

    entidades[modelocont] = e;
    modelocont++;

    return 1;
}




static int fx_entidad_es_visible(INSTANCE * my, int * params)
{
   bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

   cell_vis_t vist;
   cell_vis_t *vis= &vist;
   int visible=0;


  ray_t rays[YETI_RAY_MAX], *ray;
  cell_t* cell;
  int x = yeti.camera->x << 2;
  int y = yeti.camera->y << 2;
  int i = YETI_RAY_MAX;
  int j = (YETI_RAY_MAX >> 1) * YETI_RAY_WIDTH + yeti.camera->t;

  for (; i--; j -= YETI_RAY_WIDTH)
  {
    ray    = &rays[i];
    ray->x = x; ray->xx = fixsin(j);
    ray->y = y; ray->yy = fixcos(j);
  }

  vis->length = 0;

  for (j = YETI_RAY_MAX; j && vis->length < YETI_CELL_MAX;)
  {
    for (ray = rays, i = YETI_RAY_MAX; i--; ray++)
    {
      if (ray->x)
      {
        cell = &yeti.map.item[y = f2i(ray->y >> 2)][x = f2i(ray->x >> 2)];

        if (cell->xlod < 0 || cell->ylod < 0)
        {
          cell = &yeti.map.item[y += cell->ylod][x += cell->xlod];
        }
        if (CELL_IS_OPAQUE(cell))
        {
          j--;
          ray->x = 0;
        }
        else
        {
          //visible node to camera
          if ((f2i(be1->x) == x) &&  (f2i(be1->y) == y)   )
          {
           visible=1;
           goto salida;
          }

          ray->x += ray->xx;
          ray->y += ray->yy;
        }
      }
    }
  }



salida:
  return visible;
}



static int fx_entity_in_ground(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];
  entity_t* e= (entity_t*)   be1->yetient;

  cell_t* cell = ENTITY_CELL(&yeti, e);

  int bot = cell_bot(cell, e) +  yeti.game.walking;


  if (e->z <= bot)
  {
     //esta en tierra  --- saltar
    return 1;
  }


  return  0 ;
}



static int fx_entity_line_of_sight(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];
  bennu_entity_t* be2 =(bennu_entity_t* ) params[1];

  return  entity_line_of_sight(be1->yetient, be2->yetient, &yeti.map) ;
}


static int fx_entity_look_at(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_look_at( be1->yetient,  params[1],params[2],params[3] ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_look_at2(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_look_at2( be1->yetient,  params[1],params[2],params[3] ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


//move entities

static int fx_entity_freeze(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_freeze( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}



static int fx_entity_friction(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_friction( be1->yetient,  params[1] ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_move_forward(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_move_forward( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_move_backwards(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_move_backwards( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_move_right(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_move_right( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_move_left(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_move_left( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}


static int fx_entity_turn_right(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_turn_right( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}

static int fx_entity_turn_left(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_turn_left( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}



static int fx_entity_turn_towards(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_turn_towards( be1->yetient ,    params[1], params[2]) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}

static int fx_entity_set_velocity(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_set_velocity( be1->yetient ) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}



static int fx_entity_force_towards(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_force_towards( be1->yetient ,    params[1], params[2],params[3],params[4]) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}



static int fx_entity_default(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  Entidadcopiarayeti(be1->yetient,   be1);
  entity_default(&yeti,  be1->yetient ,    params[1], params[2],params[3]) ;
  Entidadcopiarabennu(be1,  be1->yetient);

  return 1;
}




static int fx_sprites_anim(INSTANCE * my, int * params)
{


  bennu_entity_t* be =(bennu_entity_t* ) params[0];
  entity_t *e= (entity_t *)be->yetient;


  e->animation.mode   = 0;
  e->animation.start  = params[1];
  e->animation.length = params[2];
  e->animation.speed  = params[3];
  e->animation.end    = e->animation.start + e->animation.length;
  e->animation.pos    = e->animation.start;
  e->animation.nloops = params[4];

 return 1;
}



void misprite_tick(yeti_t* yeti, entity_t* e)
{

 if (e->animation.length )
 {
  e->visual.data= ((sprite_t*) e->tag)[e->animation.pos];


  if (e->animation.nloops > 0)
    e->animation.mode+=  e->animation.speed;

  if (e->animation.mode > 70 )
  {
    e->animation.pos += 1;

    if (e->animation.pos >= e->animation.end)
    {
       e->animation.pos= e->animation.start;

       if (e->animation.nloops > 0 && (e->animation.nloops != LOOP_FOREVER))
          e->animation.nloops--;

       if (!e->animation.nloops) e->animation.pos= e->animation.end - 1;//se acabo el loop
    }

   e->animation.mode=0;
  }

 }

}





void bsprite_init(yeti_t* yeti, entity_t* e)
{
  cell_t* cell = ENTITY_CELL(yeti, e);

  e->animation.length=0;

  e->visual.width = i2f(2);
  e->visual.height = i2f(2);
  //e->visual.data = spr_horror;
  e->visual.mode = DRAW_MODE_COLORKEY;
  e->z = cell_bot(cell, e) ;
  //e->castshadow = castshadow;
  //cell->eos = eos;
  e->ontick  = misprite_tick;

}



static int fx_loadsprites3d(INSTANCE * my, int * params)
{
  const char *ruta = string_get(params[0]) ;

  int i= Load_Sprites(ruta);

  string_discard (params[0]) ;

  return i;
}

static int fx_sprites3d(INSTANCE * my, int * params)
{
    if (yeti.entities.length >=  (MAX_MODELOS - 1) ) return 0;


    bennu_entity_t *pos= (bennu_entity_t *)params[1];

    entity_t *e= yeti_entity(&yeti,  pos->x,  pos->y,  pos->z);

//--- fix collision with camera
  e->radius = 128;
//-----


    if (params[0]) {
      e->visual.data =((sprite_t*) params[0])[0];
      e->tag =  params[0];
    }

    bsprite_init    (&yeti,e);



    //memcpy(pos,(bennu_entity_t *) e, sizeof(bennu_entity_t)); //inicializa bennu entity
    Entidadcopiarabennu(pos, e);

    e->bennu_e= pos;
    pos->yetient= e; //yeti ref

    entidades[modelocont] = e;
    modelocont++;

  //  printf(" SPRITEs adicionado  \n");

    return 1;
}


static int fx_setvisualdata(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  entity_t *e= (entity_t *)be1->yetient;

  e->visual.width=  params[1];
  e->visual.height= params[2];
  e->visual.mode=   params[3];

  return 1;
}


static int fx_getvisualdata(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  entity_t *e= (entity_t *)be1->yetient;

  *(int *)params[1] = e->visual.width;
  *(int *)params[2] = e->visual.height;
  *(int *)params[3] = e->visual.mode;

  return 1;
}




static int fx_SetVisual(INSTANCE * my, int * params)
{
  YETI_RAY_WIDTH=params[0];
  YETI_RAY_MAX=  params[1];;
  YETI_CELL_MAX= params[2];

  return 1;
}



/*
  BULLET CODE

*/




void bala_tick(yeti_t* yeti, entity_t* e)
{
  if (!e->xx && !e->yy) entity_destroy(e);
}


void bala_shoot_at(entity_t* e, int x, int y, int z)
{
  entity_look_at2(e, x, y, z);
  entity_set_velocity(e);
}


int bullet_actual= -1;

inline entity_t* YETI_BULLETS(yeti_t* YETI)
{
    bullet_actual++;
    if (bullet_actual >= YETI_BULLET_MAX-1) bullet_actual=0;

    return YETI->bullets.item[bullet_actual];
}



void bala_shoot(yeti_t* yeti, entity_t* e, void *data, int collid , int collwithid )
{
  entity_t* b = YETI_BULLETS(yeti);

  entity_destroy(b);

  b->t  = e->t;
  b->p  = -e->p >> 1;

  b->x  = e->x + (b->xx >> 1);
  b->y  = e->y + (b->yy >> 1);
  b->z  = e->z + GLODWORD(yeti3d,YETI_BULLET_Z);

  entity_set_velocity(b);

if (data)
  b->visual.data    = ((sprite_t*) data)[0];
  b->visual.width   = GLODWORD(yeti3d,YETI_BULLET_WIDTH);
  b->visual.height  = GLODWORD(yeti3d,YETI_BULLET_HEIGHT);
  b->visual.mode    = GLODWORD(yeti3d,YETI_BULLET_DRAWMODE);
  b->radius = GLODWORD(yeti3d,YETI_BULLET_RADIUS);

  b->onwall = colision_wall;
  b->onhit  = colision_hit;
  b->ontick  = bala_tick;

  b->CollisionId= collid; //collision flags
  b->CollisionWithId= 0;
  b->CollisionWithId |= collwithid;

  b->CollisionOwner= e;

  b->emitlight = TRUE;
}


static int fx_bulletshot(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];
  entity_t *e= (entity_t *)be1->yetient;

  sprite_t *data=  params[1];

  bala_shoot(& yeti, e,
      data, //data
      params[2], //collisionid
      params[3] //collisionwith
        );

  return 1;
}


static int fx_set_entity_data(INSTANCE * my, int * params)
{
  bennu_entity_t* be1 =(bennu_entity_t* ) params[0];

  entity_t *e= (entity_t *)be1->yetient;

  e->visual.ondraw=0;
  e->visual.data=  params[1];
  e->ontick=0;


  if (params[2])
    modelo_stand_init    (e);
  else
  {
    if (params[1]) {
       e->visual.data =((sprite_t*) params[1])[0];
       e->tag =  params[1]; //para sprites animados
    }

    bsprite_init    (&yeti,e);
  }

  Entidadcopiarabennu(be1, e);

 // printf("set data ok  \n");

  return 1;
}





//renderiza sin actualizar entidades
static int fx_yeti_draw(INSTANCE * my, int * params)
{
        //update player data - cammera
     Entidadcopiarayeti(yeti.player, yeti.player->bennu_e);


     fx_Render(1);

     return 1;
}




void RenderizarEscena();

//renderiza  actualizando entidades
static int fx_yeti_render(INSTANCE * my, int * params)
{
     RenderizarEscena();

     return 1;
}



/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

#define FENIX_export(a,b,c,d)  {a,b,c,(void*)d}


DLLEXPORT DLSYSFUNCS  __bgdexport(yeti3d,functions_exports)[] = {
    //initialize

	FENIX_export ("YETI_INIT", "PS", TYPE_DWORD, fx_Iniciar ) ,
    FENIX_export ("YETI_FREE", "", TYPE_DWORD, fx_Terminar ) ,


    FENIX_export ("YETI_LOADTEXTUREFPG", "II", TYPE_DWORD, fx_CargarTexturaFenix ) ,
    FENIX_export ("YETI_LOADSPRITEFPG", "II", TYPE_DWORD, fx_CargarSpritesFenix ) ,
    FENIX_export ("YETI_SETRENDERGRAPH", "II", TYPE_DWORD, fx_setrendergraph ) ,
    FENIX_export ("YETI_LOADTEXTURE", "SS", TYPE_DWORD, fx_settexture ) ,
    FENIX_export ("YETI_SETRENDERFX", "I", TYPE_DWORD, fx_setrendereffect ) ,
    FENIX_export ("YETI_SETRENDERFX_RGB", "IIII", TYPE_DWORD, fx_setrendereffect_rgb ) ,
    FENIX_export ("YETI_SETVISUAL", "III", TYPE_DWORD, fx_SetVisual ) ,
    FENIX_export ("YETI_SETSKY", "I", TYPE_DWORD, fx_setsky ) ,



    FENIX_export ("YETI_LOADSPRITE", "S", TYPE_DWORD, fx_loadsprites3d ) ,
    FENIX_export ("YETI_ADDSPRITE", "IP", TYPE_DWORD, fx_sprites3d ) ,
    FENIX_export ("YETI_SPRITEANIM", "PIIII", TYPE_DWORD, fx_sprites_anim ) ,
    FENIX_export ("YETI_ADDMODEL", "IP", TYPE_DWORD, fx_newmodel ) ,
    FENIX_export ("YETI_MODELANIM", "PIIII", TYPE_DWORD, fx_modelanimation ) ,


    FENIX_export ("YETI_ENTITY_SETVISUAL", "PIII", TYPE_DWORD, fx_setvisualdata ) ,
    FENIX_export ("YETI_ENTITY_GETVISUAL", "PPPP", TYPE_DWORD, fx_getvisualdata ) ,
    FENIX_export ("YETI_ENTITY_DATA", "PII", TYPE_DWORD, fx_set_entity_data ) ,

    FENIX_export ("YETI_ENTITY_IS_VISIBLE", "P", TYPE_DWORD, fx_entidad_es_visible ) ,
    FENIX_export ("YETI_ENTITY_IN_GROUND", "P", TYPE_DWORD, fx_entity_in_ground ) ,
    FENIX_export ("YETI_ENTITY_LINE_OF_SIGHT", "PP", TYPE_DWORD, fx_entity_line_of_sight ) ,
    FENIX_export ("YETI_ENTITY_LOOK_AT", "PIII", TYPE_DWORD, fx_entity_look_at ) ,
    FENIX_export ("YETI_ENTITY_LOOK_AT2", "PIII", TYPE_DWORD, fx_entity_look_at2 ) ,
    FENIX_export ("YETI_ENTITY_FREEZE", "P", TYPE_DWORD, fx_entity_freeze ) ,
    FENIX_export ("YETI_ENTITY_FRICTION", "PI", TYPE_DWORD, fx_entity_friction ) ,
    FENIX_export ("YETI_ENTITY_MOVE_FORWARD", "P", TYPE_DWORD, fx_entity_move_forward ) ,
    FENIX_export ("YETI_ENTITY_MOVE_BACKWARDS", "P", TYPE_DWORD, fx_entity_move_backwards ) ,
    FENIX_export ("YETI_ENTITY_TURN_RIGHT", "P", TYPE_DWORD, fx_entity_turn_right ) ,
    FENIX_export ("YETI_ENTITY_TURN_LEFT", "P", TYPE_DWORD, fx_entity_turn_left ) ,
    FENIX_export ("YETI_ENTITY_MOVE_RIGHT", "P", TYPE_DWORD, fx_entity_move_right ) ,
    FENIX_export ("YETI_ENTITY_MOVE_LEFT", "P", TYPE_DWORD, fx_entity_move_left ) ,
    FENIX_export ("YETI_ENTITY_TURN_TOWARDS", "PII", TYPE_DWORD, fx_entity_turn_towards ) ,
    FENIX_export ("YETI_ENTITY_SET_VELOCITY", "P", TYPE_DWORD, fx_entity_set_velocity ) ,
    FENIX_export ("YETI_ENTITY_FORCE_TOWARDS", "PIIII", TYPE_DWORD, fx_entity_force_towards ) ,
    FENIX_export ("YETI_ENTITY_DEFAULT", "PIII", TYPE_DWORD, fx_entity_default ) ,

    FENIX_export ("YETI_ENTITY_COLLISION", "PIII", TYPE_DWORD, fx_entity_collisionEnable ) ,
    FENIX_export ("YETI_ENTITY_MSG", "II", TYPE_DWORD, fx_entity_msg ) ,



    FENIX_export ("YETI_LOADMAP", "S", TYPE_DWORD, fx_loadmap ) ,
    FENIX_export ("YETI_SAVEMAP", "S", TYPE_DWORD, fx_savemap ) ,



    FENIX_export ("YETI_GETCELL", "IIP", TYPE_DWORD, fx_obtenercelda3d ) ,
    FENIX_export ("YETI_GETCELLFROMID", "IP", TYPE_DWORD, fx_obtenercelda3ddeID ) ,
    FENIX_export ("YETI_SETCELL", "P", TYPE_DWORD, fx_modificarcelda3d ) ,
    FENIX_export ("YETI_CELLBOT", "PP", TYPE_DWORD, fx_cellbot ) ,
    FENIX_export ("YETI_CELLTOP", "PP", TYPE_DWORD, fx_celltop ) ,


    FENIX_export ("YETI_LOADFILE", "S", TYPE_DWORD, fx_loadfile ) ,

    FENIX_export ("YETI_FIXCOS", "I", TYPE_DWORD, fx_fixcos ) ,
    FENIX_export ("YETI_FIXSIN", "I", TYPE_DWORD, fx_fixsin ) ,
    FENIX_export ("YETI_FIXANGLE", "II", TYPE_DWORD, fx_fixangle ) ,
    FENIX_export ("YETI_FIXARCTAN", "II", TYPE_DWORD, fx_lupin_arctan2 ) ,
    FENIX_export ("YETI_FIXSQRT", "I", TYPE_DWORD, fx_isqrt ) ,



    FENIX_export ("YETI_ANSIC_FRICTION", "II", TYPE_DWORD, fx_ansic_friction ) ,


    FENIX_export ("YETI_BULLETSHOOT", "PIII", TYPE_DWORD, fx_bulletshot ) ,


    FENIX_export ("YETI_DRAW", "", TYPE_DWORD, fx_yeti_draw ) ,
    FENIX_export ("YETI_RENDER", "", TYPE_DWORD, fx_yeti_render ) ,


    { 0            , 0     , 0         , 0              }
};



/* ----------------------------------------------------------------- */

DLLEXPORT void __bgdexport( yeti3d, module_initialize )()
{
   GLODWORD(yeti3d,YETI_SCREEN_WIDTH)=    YETI_VIEWPORT_X2_MAX;
   GLODWORD(yeti3d,YETI_SCREEN_HEIGHT)=   YETI_VIEWPORT_Y2_MAX;



}



DLLEXPORT char * __bgdexport(yeti3d,modules_dependency)[] = {
	"mod_string",
	"libvideo",
	"libgrbase",
	NULL
};




void RenderizarEscena()
{

  //  copy to yeti entities;

      //update player data
      Entidadcopiarayeti(yeti.player, yeti.player->bennu_e);

      int i;
       #pragma omp parallel for
      for (i=0;i< modelocont ;i++) {
          Entidadcopiarayeti(entidades[i],  entidades[i]->bennu_e);
      }

       fx_Render(0);

       //update player data
      Entidadcopiarabennu( yeti.player->bennu_e, yeti.player);

       //copy to bennu entity
       #pragma omp parallel for
      for (i=0;i< modelocont ;i++) {
         // printf("i= %d", i);
          Entidadcopiarabennu( entidades[i]->bennu_e, entidades[i]);
      }

}


static void Render3DHook()
{
  if (iniciado_3d && GLODWORD(yeti3d,YETI_AUTO_RENDER))
  {
     RenderizarEscena();
  }

}

DLLEXPORT HOOK __bgdexport( yeti3d, handler_hooks )[] =
{
    { 100, Render3DHook },
    { 0, NULL }
} ;
