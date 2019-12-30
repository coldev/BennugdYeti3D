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

#include "y3d_file.h"

/******************************************************************************/

/*
** Name: file_load
** Desc: Allocates memory and loads a file into it. You must free the memory
**       after use.
*/

void* yeti_load_file(char* filename,int *size)
{
  void* mem = NULL;
  *size=0;


  FILE* fp = fopen(filename, "r");

  if (fp)
  {
    if (!fseek(fp, 0, SEEK_END))
    {
      int n = ftell(fp);
      *size= n;
      if ((mem = malloc(n)) != 0)
      {
        fseek(fp, 0, SEEK_SET);
        fread(mem, 1, n, fp);
      }
    }
    fclose(fp);
  }


    if ( !mem )
    {
      int fp = file_open( filename, "rb0" ) ;

      if (fp > 0)
      {
       *size=  file_size(fp);

       if ((mem = malloc(*size)) != 0)
       {
         file_read( fp, mem, *size );
         file_close( fp ) ;
       }
      }

    }

    printf(" file size ( %d - %d)  \n", *size,mem);

  return mem;
}



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


