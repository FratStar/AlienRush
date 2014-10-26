#include "graphics.h"
#include "ar_types.h"
#include "sprite.h"


#define MaxSprites	511

extern arSurface *screen; /*pointer to the draw buffer*/
extern arSurface *background;/*pointer to the background image buffer*/
extern arSurface *bgimage;
extern arSurface *videobuffer;
extern arRect Camera; /*x & y are the coordinates for the background map, w and h are of the screen*/
extern arUI32 NOW;  /*this represents the current time for the game loop.  Things move according to time*/

Sprite SpriteList[MaxSprites];
int NumSprites;


void InitSpriteList()
{
  int x;
  NumSprites = 0;
  memset(SpriteList,0,sizeof(Sprite) * MaxSprites);
  for(x = 0;x < MaxSprites;x++)SpriteList[x].image = NULL;
}

/*Create a sprite from a file, the most common use for it.*/

Sprite *LoadSprite(char *filename,int sizex, int sizey)
{
  int i;
  arSurface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if(strncmp(filename,SpriteList[i].filename,40)==0)
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
    fprintf(stderr,"unable to load a vital sprite: %s\n",SDL_GetError());
    exit(0);
  }
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGBA(SpriteList[i].image->format, 255,255,255,255));
   /*then copy the given information to the sprite*/
  strncpy(SpriteList[i].filename,filename,20);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 5;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
  SpriteList[i].used++;
  return &SpriteList[i];
}

Sprite *LoadSpriteEn(char *filename,int sizex, int sizey)
{
  int i;
  arSurface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if(strncmp(filename,SpriteList[i].filename,40)==0)
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
    fprintf(stderr,"unable to load a vital sprite: %s\n",SDL_GetError());
    exit(0);
  }
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGBA(SpriteList[i].image->format, 0,0,0,255));
   /*then copy the given information to the sprite*/
  strncpy(SpriteList[i].filename,filename,20);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 16;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
  SpriteList[i].used++;
  return &SpriteList[i];
}

void SwapSprite(arSurface *sprite,int color1,int color2,int color3)
{
    int x, y;
	arSurface *temp;
    arUI32 pixel,pixel2;
    arUI32 Key = sprite->format->colorkey;
   /*First the precautions, that are tedious, but necessary*/
    if(color1 == -1)return;
    if(sprite == NULL)return;
    temp = SDL_DisplayFormat(sprite);
    if ( SDL_LockSurface(temp) < 0 )
    {
        fprintf(stderr, "Can't lock surface: %s\n", SDL_GetError());
        exit(1);
    }
   /*now step through our sprite, pixel by pixel*/
    for(y = 0;y < sprite->h ;y++)
    {
        for(x = 0;x < sprite->w ;x++)
        {
            pixel = getpixel(temp,x,y);/*and swap it*/
			if(pixel != Key)
			{
			  pixel2 = SetColor(pixel,color1,color2,color3);
              putpixel(sprite,x,y,pixel2);
			}
        }
    }
    SDL_UnlockSurface(temp);
	SDL_FreeSurface(temp);
}

Sprite *LoadSwappedSprite(char *filename,int sizex, int sizey)
{
  int i;
  arSurface *temp;
  /*first search to see if the requested sprite image is alreday loaded*/
  for(i = 0; i < NumSprites; i++)
  {
    if((strncmp(filename,SpriteList[i].filename,40)==0)&&(SpriteList[i].used >= 1))
    {
      SpriteList[i].used++;
      return &SpriteList[i];
    }
  }
  /*makesure we have the room for a new sprite*/
  if(NumSprites + 1 >= MaxSprites)
  {
        fprintf(stderr, "Maximum Sprites Reached.\n");
        exit(1);
  }
  /*if its not already in memory, then load it.*/
  NumSprites++;
  for(i = 0;i <= NumSprites;i++)
  {
    if(!SpriteList[i].used)break;
  }
  temp = IMG_Load(filename);
  if(temp == NULL)
  {
        fprintf(stderr, "FAILED TO LOAD A VITAL SPRITE.\n");
        exit(1);
  }
  SpriteList[i].image = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  /*sets a transparent color for blitting.*/
  SDL_SetColorKey(SpriteList[i].image, SDL_SRCCOLORKEY , SDL_MapRGBA(SpriteList[i].image->format, 0,0,0,0));
  /*(printf(stderr,"asked for colors: %d,%d,%d \n",c1,c2,c3);*/
  /*SwapSprite(SpriteList[i].image,c1,c2,c3);*/
   /*then copy the given information to the sprite*/
  strcpy(SpriteList[i].filename,filename);
      /*now sprites don't have to be 16 frames per line, but most will be.*/
  SpriteList[i].framesperline = 5;
  SpriteList[i].w = sizex;
  SpriteList[i].h = sizey;
/*  SpriteList[i].color1 = c1;
  SpriteList[i].color2 = c2;
  SpriteList[i].color3 = c3;*/
  SpriteList[i].used++;
  return &SpriteList[i];
}

void FreeSprite(Sprite *sprite)
{
  /*first lets check to see if the sprite is still being used.*/
  sprite->used--;
  if(sprite->used == 0)
  {
  strcpy(sprite->filename,"\0");
     /*just to be anal retentive, check to see if the image is already freed*/
  if(sprite->image != NULL)SDL_FreeSurface(sprite->image);
  sprite->image = NULL;
  }
 /*and then lets make sure we don't leave any potential seg faults 
  lying around*/
}

void CloseSprites()
{
  int i;
   for(i = 0;i < MaxSprites;i++)
   {
     /*it shouldn't matter if the sprite is already freed, 
     FreeSprite checks for that*/
      FreeSprite(&SpriteList[i]);
   }
}




void DrawGreySprite(Sprite *sprite,arSurface *surface,int sx,int sy, int frame)
{
  int i,j;
  int offx,offy;
  arUI8 r,g,b, a;
  arUI32 pixel;
  arUI32 Key = sprite->image->format->colorkey;
  offx = frame%sprite->framesperline * sprite->w;
  offy = frame/sprite->framesperline * sprite->h;
  if ( SDL_LockSurface(sprite->image) < 0 )
  {
      fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
      exit(1);
  }
  for(j = 0;j < sprite->h;j++)
  {
    for(i = 0;i < sprite->w;i++)
    {
      pixel = getpixel(sprite->image, i + offx ,j + offy);
      if(Key != pixel)
      {
        SDL_GetRGBA(pixel, sprite->image->format, &r, &g, &b, &a);
        r = (r + g + b + a)/3;
        putpixel(surface, sx + i, sy + j, SDL_MapRGBA(sprite->image->format, r, r, r, r));
      }
    }
  }
  SDL_UnlockSurface(sprite->image);
}

void DrawSprite(Sprite *sprite,arSurface *surface,int sx,int sy, int frame)
{
    arRect src,dest;
    src.x = frame%sprite->framesperline * sprite->w;
    src.y = frame/sprite->framesperline * sprite->h;
    src.w = sprite->w;
    src.h = sprite->h;
    dest.x = sx;
    dest.y = sy;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

void DrawSpritePixel(Sprite *sprite,arSurface *surface,int sx,int sy, int frame)
{
    arRect src,dest;
    src.x = frame%sprite->framesperline * sprite->w + sprite->w/2;
    src.y = frame/sprite->framesperline * sprite->h + sprite->h/2;
    src.w = 1;
    src.h = 1;
    dest.x = sx;
    dest.y = sy;
    dest.w = 1;
    dest.h = 1;
    SDL_BlitSurface(sprite->image, &src, surface, &dest);
}

arUI32 SetColor(arUI32 color, int newcolor1,int newcolor2, int newcolor3)
{
    arUI8 r,g,b, a;
    arUI8 intensity;
	int newcolor;
    SDL_GetRGBA(color, screen->format, &r, &g, &b, &a);
    if((r == 0) && (g == 0) && (a == 255) && (b !=0))
    {
        intensity = b;
        newcolor = newcolor3;
    }
    else if((r ==0) && (b == 0) && (g != 0) && (a == 255))
    {
        intensity = g;
        newcolor = newcolor2;
    }
    else if((g == 0) && (b == 0) && (a == 255) && (r != 0))
    {
        intensity = r;
        newcolor = newcolor1;
    }
    else return color;
    switch(newcolor)
    {
        case Red:
            r = intensity;
            g = 0;
            b = 0;
			a = 255;
            break;
        case Green:
            r = 0;
            g = intensity;
            b = 0;
			a = 255;
            break;
        case Blue:
            r = 0;
            g = 0;
            b = intensity;
			a = 255;
            break;
        case Yellow:
            r = (arUI8)(intensity * 0.7);
            g = (arUI8)(intensity * 0.7);
            b = 0;
			a = 255;
            break;
        case Orange:
            r = (arUI8)(intensity * 0.9);
            g = (arUI8)(intensity * 0.4);
            b = (arUI8)(intensity * 0.1);
			a = 255;
            break;
        case Violet:
            r = (arUI8)(intensity * 0.7);
            g = 0;
            b = (arUI8)(intensity * 0.7);
			a = 255;
            break;
        case Brown:
            r = (arUI8)(intensity * 0.6);
            g = (arUI8)(intensity * 0.3);
            b = (arUI8)(intensity * 0.15);
			a = 255;
            break;
        case Grey:
            r = (arUI8)(intensity * 0.5);
            g = (arUI8)(intensity * 0.5);
            b = (arUI8)(intensity * 0.5);
			a = 255;
            break;
        case DarkRed:
            r = (arUI8)(intensity * 0.5);
            g = 0;
            b = 0;
			a = 255;
            break;
        case DarkGreen:
            r = 0;
            g = (arUI8)(intensity * 0.5);
            b = 0;
			a = 255;
            break;
        case DarkBlue:
            r = 0;
            g = 0;
            b = (arUI8)(intensity * 0.5);
			a = 255;
            break;
        case DarkYellow:
            r = (arUI8)(intensity * 0.4);
            g = (arUI8)(intensity * 0.4);
            b = 0;
			a = 255;
            break;
        case DarkOrange:
            r = (arUI8)(intensity * 0.6);
            g = (arUI8)(intensity * 0.2);
            b = (arUI8)(intensity * 0.1);
			a = 255;
            break;
        case DarkViolet:
            r = (arUI8)(intensity * 0.4);
            g = 0;
            b = (arUI8)(intensity * 0.4);
			a = 255;
            break;
        case DarkBrown:
            r = (arUI8)(intensity * 0.2);
            g = (arUI8)(intensity * 0.1);
            b = (arUI8)(intensity * 0.05);
			a = 255;
            break;
        case DarkGrey:
            r = (arUI8)(intensity * 0.3);
            g = (arUI8)(intensity * 0.3);
            b = (arUI8)(intensity * 0.3);
			a = 255;
            break;
        case LightRed:
            r = intensity;
            g = (arUI8)(intensity * 0.45);
            b = (arUI8)(intensity * 0.45);
			a = 255;
            break;
        case LightGreen:
            r = (arUI8)(intensity * 0.45);
            g = intensity;
            b = (arUI8)(intensity * 0.45);
			a = 255;
            break;
        case LightBlue:
            r = (arUI8)(intensity * 0.45);
            b = intensity;
            g = (arUI8)(intensity * 0.45);
			a = 255;
            break;
        case LightYellow:
            r = intensity;
            g = intensity;
            b = (arUI8)(intensity * 0.45);
			a = 255;
            break;
        case LightOrange:
            r = intensity;
            g = (arUI8)(intensity * 0.75);
            b = (arUI8)(intensity * 0.35);
			a = 255;
            break;
        case LightViolet:
            r = intensity;
            g = (arUI8)(intensity * 0.45);
            b = intensity;
			a = 255;
            break;
        case LightBrown:
            r = intensity;
            g = (arUI8)(intensity * 0.85);
            b = (arUI8)(intensity * 0.45);
			a = 255;
            break;
        case LightGrey:
            r = (arUI8)(intensity * 0.85);
            g = (arUI8)(intensity * 0.85);
            b = (arUI8)(intensity * 0.85);
			a = 255;
            break;
        case Black:
            r = (arUI8)(intensity * 0.15);
            g = (arUI8)(intensity * 0.15);
            b = (arUI8)(intensity * 0.15);
			a = 255;
            break;
        case White:
            r = intensity;
            g = intensity;
            b = intensity;
			a = 255;
            break;
        case Tan:
            r = intensity;
            g = (arUI8)(intensity * 0.9);
            b = (arUI8)(intensity * 0.6);
			a = 255;
            break;
        case Gold:
            r = (arUI8)(intensity * 0.8);
            g = (arUI8)(intensity * 0.7);
            b = (arUI8)(intensity * 0.2);
			a = 255;
            break;
        case Silver:
            r = (arUI8)(intensity * 0.95);
            g = (arUI8)(intensity * 0.95);
            b = intensity;
			a = 255;
            break;
        case YellowGreen:
            r = (arUI8)(intensity * 0.45);
            g = (arUI8)(intensity * 0.75);
            b = (arUI8)(intensity * 0.2);
			a = 255;
            break;
        case Cyan:
            r = 0;
            g = (arUI8)(intensity * 0.85);
            b = (arUI8)(intensity * 0.85);
			a = 255;
            break;
        case Magenta:
            r = (arUI8)(intensity * 0.7);
            g = 0;
            b = (arUI8)(intensity * 0.7);
			a = 255;
            break;
		default:
            r = 0;
            g = (arUI8)(intensity * 0.85);
            b = (arUI8)(intensity * 0.85);
			a = 255;
			break;
    }
	color = SDL_MapRGBA(screen->format,r,g,b,a);
//    fprintf(stderr,"newcolor: %d, asked for: %d,%d,%d \n",color,newcolor1,newcolor2,newcolor3); 
    return color;
}

arUI32 IndexColor(int color)
{
    switch(color)
    {
    case Red:
        return SDL_MapRGBA(screen->format,138,0,0,255);;
    case Green:
        return SDL_MapRGBA(screen->format,0,138,0,255);;
    case Blue:
        return SDL_MapRGBA(screen->format,0,0,138,255);;
    case Yellow:
        return SDL_MapRGBA(screen->format,128,128,0,255);;
    case Orange:
        return SDL_MapRGBA(screen->format,148,118,0,255);;
    case Violet:
        return SDL_MapRGBA(screen->format,128,0,128,255);
    case Brown:
        return SDL_MapRGBA(screen->format,100,64,4,255);
    case Grey:
        return SDL_MapRGBA(screen->format,128,128,128,255);
    case DarkRed:
        return SDL_MapRGBA(screen->format,64,0,0,255);
    case DarkGreen:
        return SDL_MapRGBA(screen->format,0,64,0,255);
    case DarkBlue:
        return SDL_MapRGBA(screen->format,0,0,64,255);
    case DarkYellow:
        return SDL_MapRGBA(screen->format,60,60,0,255);
    case DarkOrange:
        return SDL_MapRGBA(screen->format,64,56,0,255);
    case DarkViolet:
        return SDL_MapRGBA(screen->format,60,0,60,255);
    case DarkBrown:
        return SDL_MapRGBA(screen->format,56,32,2,255);
    case DarkGrey:
        return SDL_MapRGBA(screen->format,64,64,64,255);
    case LightRed:
        return SDL_MapRGBA(screen->format,255,32,32,255);
    case LightGreen:
        return SDL_MapRGBA(screen->format,32,255,32,255);
    case LightBlue:
        return SDL_MapRGBA(screen->format,32,32,255,255);
    case LightYellow:
        return SDL_MapRGBA(screen->format,250,250,60,255);
    case LightOrange:
        return SDL_MapRGBA(screen->format,255,234,30,255);
    case LightViolet:
        return SDL_MapRGBA(screen->format,250,30,250,255);
    case LightBrown:
        return SDL_MapRGBA(screen->format,200,100,32,255);
    case LightGrey:
        return SDL_MapRGBA(screen->format,196,196,196,255);
    case Black:
        return SDL_MapRGBA(screen->format,0,0,0,255);
    case White:
        return SDL_MapRGBA(screen->format,255,255,255,255);
    case Tan:
        return SDL_MapRGBA(screen->format,255,128,64,255);
    case Gold:
        return SDL_MapRGBA(screen->format,255,245,30,255);
    case Silver:
        return SDL_MapRGBA(screen->format,206,206,226,255);
    case YellowGreen:
        return SDL_MapRGBA(screen->format,196,255,30,255);
    case Cyan:
        return SDL_MapRGBA(screen->format,0,255,255,255);;
    case Magenta:
        return SDL_MapRGBA(screen->format,255,0,255,255);
    }
    return SDL_MapRGBA(screen->format,0,0,0,255);
}

