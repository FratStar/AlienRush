#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "graphics.h"
#include "sprite.h"
#include "audio.h"
#include "entity.h"
#include "enemy.h"
#include "level.h"
#include "particle.h"
/*#include "projectiles.h"*/
#include "player.h"
#include "weapons.h"
#include "menu.h"
#include "spawn.h"
#include "sprite.h"
#include "space.h"
//#include "worldents.h"
#include "ar_types.h"

#define MAXSTATE 1


extern arSurface *screen;
extern arSurface *bgimage;
extern arSurface *background;
extern arRect Camera;
extern Entity *ThePlayer;
extern Entity *Evil;
extern Level level;
extern arSurface *clipmask;
int lvl = 0;
int drawents = 1,drawboxes = 0;
int windowed = 1;
int mapeditmode = 0;
int bucketdraw = 0;
Sound *gamesounds[4];
SDL_Joystick *joy;
arUI8 *keys;

void Init_All();
void Update_ALL();
int Think_ALL();
void Draw_ALL();
void DrawSplashScreen();
void GiveInfo();
void PopUpWindow(Sprite *sprite,char *text,arUI32 Color,int pwait);
void UpdateCamera();
void UpdateMapCamera();





int main(int argc, char *argv[])
{
	FILE *file = NULL;
	int done = 0;
	int i;
	char string[40];
	SDLMod mod;
	string[0] = '\0';
	for(i = 1;i < argc;i++)
	{
		if(strcmp("-nodraw",argv[i])== 0)
		{
			drawents = 0;
			drawboxes = 1;
		}
		else if(strcmp("-windowed",argv[i])== 0)
		{
			windowed = 1;
		}
		else if(strcmp("-drawall",argv[i])== 0)
		{
			drawents = 1;
			drawboxes = 1;
		}
		else if(strcmp("-mapedit",argv[i])== 0)
		{
			mapeditmode = 1;
		}
		else if(strcmp("-buckets",argv[i])== 0)
		{
			bucketdraw = 1;
		}
	}
	Init_All();
	if(lvl==1)printf("woot");
	GiveInfo();
	if(!mapeditmode)
	{
		SpawnAll(1);
		if(ThePlayer == NULL)SpawnPlayer(128,128);
		/*if(Evil == NULL)Spawnenemy(Evil,140,40,1,1);*/
	}
	do
	{
		ResetBuffer();
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);    
		mod = SDL_GetModState();
		Draw_ALL();
		if(keys[SDLK_ESCAPE] == 1)done = 1;
		Update_ALL();
		if(!mapeditmode)
		{
			if(keys[SDLK_F1] == 1)
			{
				PlayerConfig(ThePlayer);
			}
			Think_ALL();
		}
		/*else
		{
			if((keys[SDLK_k] == 1)&&(mod & KMOD_SHIFT))
			{
				SDL_PumpEvents();
				if(GetString(Draw_ALL,"Enter Map Name",string,40))
				{  
					SaveLevel(string);
					NewMessage("Map Saved",IndexColor(LightBlue));
				}
				else NewMessage("Map Save Cancelled",IndexColor(LightRed));
			}
			if((keys[SDLK_l] == 1)&&(mod & KMOD_SHIFT))
			{
				SDL_PumpEvents();
				if(GetString(Draw_ALL,"Enter Map Name",string,40))
				{
					file = fopen(string, "r");
					if(file != NULL)
					{
						fclose(file);
						NewMessage("Map Loaded",IndexColor(LightBlue));
						LoadLevel(string);
						DrawLevel();
						Mix_HaltMusic();
					}
					else NewMessage("Unable to Open Map",IndexColor(LightRed));
				}
				else NewMessage("Map Load Cancelled",IndexColor(LightRed));
			}    
		}*/
		if(keys[SDLK_b] == 1)
		{
			SDL_SaveBMP(screen,"screen0.bmp");
			NewMessage("Screen Shot Saved",IndexColor(LightBlue));
		}
		NextFrame();
	}while(!done);

	exit(0);
	return 0;
}


void CleanUpAll()
{
	CloseSprites();
	ClearEntities();
	ClearRegionMask();
	ClearSoundList();
	if(SDL_JoystickOpened(0))
		SDL_JoystickClose(joy);
	SDL_FreeSurface(clipmask);
	/*any other cleanup functions can be added here*/ 
}


void Init_All()
{
	Init_Graphics(windowed);
	InitSpriteList();
	Init_Audio();
	InitSoundList();
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	atexit(CleanUpAll);
	LoadFonts();
	InitMessages();
	InitEntityList();
	ResetAllParticles();
	LoadHUD();
	if(mapeditmode)
	{
		LoadMouse();
		GenerateLevel(128,18);
		InitRegionMask(128,18);
		LoadDashboard();
		LoadSpawnSprites();
	}
	else
	{
		MainMenu();
		//CreateLevel("Level1",128,18,"images/tiles/dirt.png",0,1);
		GenerateLevel1("system/level.def",128,18,lvl);
		//LoadLevel("maps/newmap.amp");
	//	InitRegionMask(level.width,level.height); /*based on size of map*/

		LoadKeyConfig();
	}
	DrawSplashScreen();

	DrawLevel();
	if(mapeditmode)Mix_HaltMusic();
	/*PrecacheProjSounds();*/
	PrecacheWeaponSounds();
	if(SDL_NumJoysticks()>0)
	{
		joy=SDL_JoystickOpen(0);

		if(joy)
		{
			printf("Opened Joystick 0\n");
			printf("Name: %s\n", SDL_JoystickName(0));
			printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
			printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
			printf("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
			printf("Number of Hats: %d\n", SDL_JoystickNumHats(joy));
		}
		else
			printf("Couldn't open Joystick 0\n");

	}

}




void GiveInfo()
{
	NewMessage("Press Esc to Quit",IndexColor(White));
}

void PopUpWindow(Sprite *splash,char *text,Uint32 Color,int frame)
{
	Sprite *popup;
	int len = strlen(text);
	popup = LoadSprite("images/popup.png",400,300);
	DrawSprite(popup,screen,(screen->w - 400)/2,(screen->h - 300)/2,0);
	if(splash != NULL)
		DrawSprite(splash,screen,(screen->w - splash->w)/2,(screen->h - splash->h)/2,frame);
	if(text != NULL)
		DrawText(text,screen,(screen->w - (len *11))/2 ,(screen->h - 300)/2 + 50,Color,F_Large);
	DrawText("Hit Spacebar to continue",screen,(screen->w)/2 - 136 ,(screen->h/2) + 100,Color,F_Large);
	NextFrame();
	FreeSprite(splash);
	FreeSprite(popup);
	do
	{
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);
	}
	while(keys[SDLK_SPACE] != 1);

}

void DrawSplashScreen()
{
	SDL_Surface *splash;
	splash = IMG_Load("images/lvlsplash.png");
	if(splash != NULL)
	{
		SDL_BlitSurface(splash,NULL,screen,NULL);
		NextFrame();
	}
	SDL_FreeSurface(splash);
	do
	{
		SDL_PumpEvents();
		keys = SDL_GetKeyState(NULL);
	}
	while((SDL_GetTicks() < 2000)&&(keys[SDLK_SPACE] != 1));
}

/*calls all of the update functions for everything*/
void Update_ALL()
{
	int i;
	Uint8 *keys;
	if(!mapeditmode)
	{
		UpdateEntities();
		UpdateCamera();
	}
	else
	{
		UpdateMouse();
		UpdateDashboard();
		UpdateMapCamera();
	}
}

/*calls all of the think function for everything*/
int Think_ALL()
{
	int done = 0;
	ThinkEntities();
	return done;
}

/*calls all of the draw functions for everything*/
void Draw_ALL()
{
	if(!mapeditmode)
	{
		DrawAllParticles();
	}
	if(drawents)
	{
		DrawEntities();
		if((!mapeditmode)&&(ThePlayer != NULL))DrawEntity(ThePlayer);
	}
	if(drawboxes)DrawBBoxEntities();
	DrawMessages();
	if(!mapeditmode)DrawHUD(ThePlayer);
	else
	{
		DrawSpawnPoints();
		DrawDashboard();
		DrawMouse();
	}
	if(bucketdraw)DrawBuckets();
}



void UpdateMapCamera()
{
	int mx,my;
	SDL_GetMouseState(&mx,&my);
	if((mx < 10)&&(Camera.x >= 10))  Camera.x -= 10;
	if((mx > screen->w - 10)&&(Camera.x <= (background->w - screen->w) - 10))Camera.x += 10;
	if((my < 10)&&(Camera.y >= 10))  Camera.y -= 10;
	if((my > screen->h - 10)&&(Camera.y <= (background->h - (screen->h - 100)) - 10))Camera.y += 10;
}

void UpdateCamera()
{
	Camera.x = (int)ThePlayer->s.x - (Camera.w >> 1);
	Camera.y = (int)ThePlayer->s.y - (Camera.h >> 1);
	if(Camera.x > background->w - Camera.w)Camera.x = background->w - Camera.w;
	if(Camera.x < 0)Camera.x = 0;
	if(Camera.y > background->h - Camera.h)Camera.y = background->h - Camera.h;
	if(Camera.y < 0)Camera.y = 0;

}