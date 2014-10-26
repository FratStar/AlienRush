#ifndef _GRAPHICS_
#define _GRAPHICS_

#include "SDL.h"
#include "SDL_image.h"
#include "Ar_types.h"

/*color constants:*/

enum FONTS {F_Small, F_Medium, F_Large};



typedef struct
{
    int xres,yres,depth;
}ScreenData;


/*the basics*/
void Init_Graphics(int windowed);
void DrawPixel(arSurface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y);
void DrawSquareLine(arSurface *screen,Uint32 color,int sx,int sy,int gx,int gy);
void ShowImage(arSurface *image, arSurface *screen, int x, int y);
Uint32 getpixel(arSurface *surface, int x, int y);
void putpixel(arSurface *surface, int x, int y, Uint32 pixel);
void DrawFilledRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface);
void DrawRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface);
void DrawElipse(int ox,int oy, int radius, Uint32 Color, arSurface *surface);
void DrawAnyLine(int sx,int sy,int dx, int dy,Uint32 Color,arSurface *surface);
void DrawThickLine(int sx,int sy,int dx, int dy,int width,Uint32 Color,arSurface *surface);


/*frame handling functions*/
void BlankScreen(arSurface *buf,Uint32 color);
void FrameDelay(Uint32 delay);
void ResetBuffer();
void NextFrame();


/*font stuff*/
void LoadFonts();
void LoadFont(char filename[40],int ptsize,int type);
void DrawText(char *text,arSurface *surface,int sx,int sy,Uint32 color,int size);
void DrawTextCentered(char *text,arSurface *surface,int sx,int sy,Uint32 color,int size);
void DrawMessages();
void InitMessages();
void NewMessage(char *text,Uint32 color);

#endif

