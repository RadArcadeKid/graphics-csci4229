/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#include "CSCIx229.h"

#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   glColor3f(	0.576, 0.439, 0.859);
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch){
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
    }
}
