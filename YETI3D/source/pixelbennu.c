
#include "y3d_viewport.h"
#include "y3d_pixel.h"
#include "y3d_draw.h"

typedef  void (*funcionrender3d_t)() ;

funcionrender3d_t funcionrender3d;



typedef struct pixel_buffer_800
{
  pixel_t pixels[800][600];

} pixel_buffer_800;



typedef struct pixel_buffer_640
{
  pixel_t pixels[640][480];

} pixel_buffer_640;



typedef struct pixel_buffer_320
{
  pixel_t pixels[320][240];

} pixel_buffer_320;


typedef struct pixel_buffer_160
{
  pixel_t pixels[160][120];

} pixel_buffer_160;



typedef struct pixel_buffer_80
{
  pixel_t pixels[80][60];

} pixel_buffer_80;




void pixel_buffer_draw320(
  pixel_buffer_320 * pixelbuffer,
  u16* video, int pitch,
  pixel_converter_t* pixel_converter)
{
  if (pixel_converter)
  {
    int y;

    for (y = 0; y < YETI_FRAMEBUFFER_HEIGHT; y++)
    {
      u16* fb = video;
      u16* tb = pixelbuffer->pixels[y];
      int i = YETI_FRAMEBUFFER_WIDTH;
#define AFFINE(I) fb[I] = pixel_converter->item[*tb++];
      AFFINE_LOOP()
#undef AFFINE
;
      video = (u16*)((int)video + pitch);
    }
  }
}


void render3d_320()
{
  pixel_buffer_draw320(yeti.viewport.back, (rgb555_t*)pantalla->data, pantalla->pitch, &pixel_converter);
}

