#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "graphics.h"


arSurface *screen; /*pointer to the draw buffer*/
arSurface *background;/*pointer to the background image buffer*/
arSurface *bgimage;
arSurface *videobuffer;
arRect Camera; /*x & y are the coordinates for the background map, w and h are of the screen*/
arUI32 NOW;  /*this represents the current time for the game loop.  Things move according to time*/



/*some data on the video settings that can be useful for a lot of functions*/
arUI32 rmask,gmask,bmask,amask;
ScreenData  S_Data;

void Init_Graphics(int windowed)
{
  arUI32 Vflags = SDL_ANYFORMAT | SDL_SRCALPHA;
    arUI32 HWflag = 0;
    arSurface *temp;
    S_Data.xres = 1280;
    S_Data.yres = 720;
    if(!windowed)Vflags |= SDL_FULLSCREEN;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF) < 0 )
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    if(SDL_VideoModeOK(1280, 720, 32, Vflags | SDL_HWSURFACE))
    {
        S_Data.xres = 1280;
        S_Data.yres = 720;
        S_Data.depth = 32;
		    fprintf(stderr,"32 bits of depth\n");
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1280, 720, 16, Vflags | SDL_HWSURFACE))
    {
        S_Data.xres = 1280;
        S_Data.yres = 720;
        S_Data.depth = 16;
     		fprintf(stderr,"16 bits of depth\n");
        HWflag = SDL_HWSURFACE;
    }
    else if(SDL_VideoModeOK(1280, 720, 16, Vflags))
    {
        S_Data.xres = 1280;
        S_Data.yres = 720;
        S_Data.depth = 16;
    		fprintf(stderr,"16 bits of depth\n");
        HWflag = SDL_SWSURFACE;
    }
    else                                                         
    {
        fprintf(stderr, "Unable to Use your crap: %s\n Upgrade \n", SDL_GetError());
        exit(1);
    }
    videobuffer = SDL_SetVideoMode(S_Data.xres, S_Data.yres,S_Data.depth, Vflags);
    if ( videobuffer == NULL )
    {
        fprintf(stderr, "Unable to set 1280x720 video: %s\n", SDL_GetError());
        exit(1);
    }
    temp = SDL_CreateRGBSurface(HWflag, S_Data.xres, S_Data.yres, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    screen = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    temp = SDL_CreateRGBSurface(HWflag, 2048, 2048, S_Data.depth,rmask, gmask,bmask,amask);
    if(temp == NULL)
	  {
        fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
        exit(1);
	  }
    /* Just to make sure that the surface we create is compatible with the screen*/
    background = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_ShowCursor(SDL_DISABLE);
    Camera.x = 0;
    Camera.y = 0;
    Camera.w = screen->w;
    Camera.h = screen->h;
    srand(SDL_GetTicks());
}

void ResetBuffer()
{
  /*Blit BGimage to background and then blit the tile map to that. for paralax*/
  SDL_BlitSurface(bgimage,&Camera,screen,NULL);
  SDL_BlitSurface(background,&Camera,screen,NULL);
}



void NextFrame()
{
  arUI32 Then;
  SDL_BlitSurface(screen,NULL,videobuffer,NULL);
  SDL_Flip(videobuffer);
  FrameDelay(30);
  Then = NOW;
  NOW = SDL_GetTicks();
  /* fprintf(stdout,"Ticks passed this frame: %i\n", NOW - Then);*/
}

void DrawPixel(arSurface *screen, Uint8 R, Uint8 G, Uint8 B, int x, int y)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);

    if ( SDL_LockSurface(screen) < 0 )
    {
      return;
    }
    switch (screen->format->BytesPerPixel)
    {
        case 1:
        { /* Assuming 8-bpp */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
        break;

        case 2:
        { /* Probably 15-bpp or 16-bpp */
            Uint16 *bufp;

            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
        break;

        case 3:
        { /* Slow 24-bpp mode, usually not used */
            Uint8 *bufp;

            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Gshift/8) = G;
            *(bufp+screen->format->Bshift/8) = B;
        }
        break;

        case 4:
        { /* Probably 32-bpp */
            Uint32 *bufp;

            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
        break;
    }
    SDL_UnlockSurface(screen);
    SDL_UpdateRect(screen, x, y, 1, 1);
}

/*
  Copied from SDL's website.  I use it for palette swapping
  Its not plagerism if you document it!
*/
void DrawSquareLine(arSurface *screen,Uint32 color,int sx,int sy,int gx,int gy)
{ 
  arRect box;
  if(sx < gx)box.x = sx;
  else box.x = gx;
  if(sy < gy)box.y = sy;
  else box.y = gy;
  if(sy == gy)
  {
    box.w = fabs(sx - gx);
    box.h = 1;                                        
    SDL_FillRect(screen,&box,color);    
    return;
  }
  box.h = fabs(sy - gy);
  box.w = 1;                                        
  SDL_FillRect(screen,&box,color);    
}

Uint32 getpixel(arSurface *surface, int x, int y)
{
    /* Here p is the address to the pixel we want to retrieve*/
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    if((x < 0)||(x >= surface->w)||(y < 0)||(y >= surface->h))return -1;
    switch(surface->format->BytesPerPixel)
    {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /*shouldn't happen, but avoids warnings*/
    }
}

void putpixel(arSurface *surface, int x, int y, Uint32 pixel)
{
  arRect point = {0,0,1,1};
  point.x = x;
  point.y = y;
  SDL_FillRect(surface,&point,pixel);
}

void DrawThickLine(int sx,int sy,int dx, int dy,int width,Uint32 Color,arSurface *surface)
{
  arRect box;
  int deltax,deltay;
  int x,y,curpixel;
  int den,num,numadd,numpixels;
  int xinc1,xinc2,yinc1,yinc2;
  box.w = width;
  box.h = width;
  deltax = fabs(dx - sx);        // The difference between the x's
  deltay = fabs(dy - sy);        // The difference between the y's
  x = sx;                       // Start x off at the first pixel
  y = sy;                       // Start y off at the first pixel

  if (dx >= sx)                 // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (dy >= sy)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    box.x = x;
    box.y = y;
    SDL_FillRect(surface,&box,Color);    
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}

void DrawAnyLine(int sx,int sy,int dx, int dy,Uint32 Color,arSurface *surface)
{
  int deltax,deltay;
  int x,y,curpixel;
  int den,num,numadd,numpixels;
  int xinc1,xinc2,yinc1,yinc2;
  deltax = fabs(dx - sx);        // The difference between the x's
  deltay = fabs(dy - sy);        // The difference between the y's
  x = sx;                       // Start x off at the first pixel
  y = sy;                       // Start y off at the first pixel

  if (dx >= sx)                 // The x-values are increasing
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          // The x-values are decreasing
  {
    xinc1 = -1;
    xinc2 = -1;
  }

  if (dy >= sy)                 // The y-values are increasing
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          // The y-values are decreasing
  {
    yinc1 = -1;
    yinc2 = -1;
  }

  if (deltax >= deltay)         // There is at least one x-value for every y-value
  {
    xinc1 = 0;                  // Don't change the x when numerator >= denominator
    yinc2 = 0;                  // Don't change the y for every iteration
    den = deltax;
    num = deltax >> 1;
    numadd = deltay;
    numpixels = deltax;         // There are more x-values than y-values
  }
  else                          // There is at least one y-value for every x-value
  {
    xinc2 = 0;                  // Don't change the x for every iteration
    yinc1 = 0;                  // Don't change the y when numerator >= denominator
    den = deltay;
    num = deltay >> 1;
    numadd = deltax;
    numpixels = deltay;         // There are more y-values than x-values
  }

  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    putpixel(surface,x, y,Color);             // Draw the current pixel
    num += numadd;              // Increase the numerator by the top of the fraction
    if (num >= den)             // Check if numerator >= denominator
    {
      num -= den;               // Calculate the new numerator value
      x += xinc1;               // Change the x as appropriate
      y += yinc1;               // Change the y as appropriate
    }
    x += xinc2;                 // Change the x as appropriate
    y += yinc2;                 // Change the y as appropriate
  }
}

/*
  copied and pasted and then significantly modified from the sdl website.  
  I kept ShowBMP to test my program as I wrote it, and I rewrote it to use any file type supported by SDL_image
*/

void ShowBMP(arSurface *image, arSurface *screen, int x, int y)
{
    arRect dest;

    /* Blit onto the screen surface.
       The surfaces should not be locked at this point.
     */
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;
    SDL_BlitSurface(image, NULL, screen, &dest);

    /* Update the changed portion of the screen */
    SDL_UpdateRects(screen, 1, &dest);
}


/*
  makes sure a minimum number of ticks is waited between frames
  this is to ensure that on faster machines the game won't move so fast that
  it will look terrible.
  This is a very handy function in game programming.
*/

void FrameDelay(Uint32 delay)
{
    static Uint32 pass = 100;
    Uint32 dif;
    dif = SDL_GetTicks() - pass;
    if(dif < delay)SDL_Delay( delay - dif);
    pass = SDL_GetTicks();
}


/*draws an elipse at the location specified*/
void DrawElipse(int ox,int oy, int radius, Uint32 Color, arSurface *surface)
{
  int r2 = radius * radius;
  int x,y;
  for(x = radius * -1;x <= radius;x++)
  {
    y = (int) (sqrt(r2 - x*x) * 0.6);
    putpixel(surface, x + ox, oy + y, Color);
    putpixel(surface, x + ox, oy - y, Color);
  }
}

/*draws an rectangle outline at the coordinates of the width and height*/
void DrawRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface)
{
  arRect box;
    box.x = sx;
    box.y = sy;
    box.w = sw;
    box.h = 1;                                        
    SDL_FillRect(surface,&box,Color);
    box.y = sy + sh;
    SDL_FillRect(surface,&box,Color);
    box.y = sy;
    box.w = 1;
    box.h = sh;
    SDL_FillRect(surface,&box,Color);
    box.x = sx + sw;
    SDL_FillRect(surface,&box,Color);
}

/*draws a filled rect at the coordinates, in the color, on the surface specified*/
void DrawFilledRect(int sx,int sy, int sw, int sh, Uint32 Color, arSurface *surface)
{
  arRect dst;
  dst.x = sx;
  dst.y = sy;
  dst.w = sw;
  dst.h = sh;
  SDL_FillRect(surface,&dst,Color);
}

/*sets an sdl surface to all color.*/

void BlankScreen(arSurface *buf,Uint32 color)
{
    SDL_LockSurface(buf);
    memset(buf->pixels, (Uint8)color,buf->format->BytesPerPixel * buf->w *buf->h);
    SDL_UnlockSurface(buf);
}

