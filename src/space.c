#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "space.h"
#include "entity.h"
#include "audio.h"
#include "sprite.h"
#include "level.h"

extern ScreenData  S_Data;
extern Uint32 rmask,gmask,bmask,amask;
extern SDL_Rect Camera;
extern SDL_Surface *bgimage;
extern SDL_Surface *background;
extern int lvl;
SDL_Surface *clipmask;
Level level;
Mix_Music *BgMusic = NULL;
int NumTiles = 0;


void CloseLevel()
{
	/* if(BgMusic != NULL)
	{
	Mix_FreeMusic(BgMusic);
	}*/
	if(bgimage != NULL)
	{
		SDL_FreeSurface(bgimage);
	}
	if(clipmask != NULL)
	{
		SDL_FreeSurface(clipmask);
	}
}


void GenerateLevel(int x, int y)
{
	int i,j;
	FILE *file;
	char *c;
	char buf[512];
	if(x > SPACE_W)x = SPACE_W;
	if(y > SPACE_H)y = SPACE_H;
	level.width = x;
	level.height = y;
	level.tileset = 1;
	level.infocount = 0;
	level.spawncount = 0;
	strcpy(level.levelname,"Level" + lvl);
	strcpy(level.bgimage,"images/bgimages/bgimage1.png");
	/*strcpy(level.bgmusic,"sounds/bgmusic1.ogg");*/
	memset(level.tilemap,0,sizeof(level.tilemap));



	for(j = 0;j < y;j++)
	{
		level.tilemap[j][0] = 1;
		level.tilemap[j][x - 1] = 1;    
	}
	for(i = 0;i < x;i++)
	{
		level.tilemap[0][i] = 1;
		level.tilemap[y - 1][i] = 1;    
	}
}

void GenerateLevel1(char filename[80], int x, int y, int lvl)
{
	int i,j,k,l;
	FILE *file;
	char buf[512];
	if(x > SPACE_W)x = SPACE_W;
	if(y > SPACE_H)y = SPACE_H;
	level.width = x;
	level.height = y;
	level.tileset = 1;
	level.infocount = 0;
	level.spawncount = 0;
	strcpy(level.levelname,"Level" + lvl);
	strcpy(level.bgimage,"images/bgimages/bgimage1.png");
	/*strcpy(level.bgmusic,"sounds/bgmusic1.ogg");*/
	file = fopen(filename,"r");
	if(file == NULL)
	{
		fprintf(stderr,"Unable to open map object definition file");
		return;
	}

	memset(level.tilemap,0,sizeof(level.tilemap));
	NumTiles = 0;
	i = 0;
	j = 0;
	k = 0;
	l = 0;
	while(fscanf(file, "%s", buf) != EOF)
	{
		if(strcmp(buf, "#") == 0)
		{
			fgets(buf, sizeof(buf), file);
			continue;
		}
		else if(strcmp(buf,"<level>") == 0)
		{
			fgets(buf,sizeof(buf), file);
			continue;
		}

		
		else if(buf[0] == '1')
		{
			for(l = 0; l < NELEMS(buf); l++)
			{
				if(buf[l] == '1')
				{
					level.tilemap[j][i] = 1;
					NumTiles++;
					i++;
				}
				else if(buf[l] == '0')
				{
					i++;
				}
				else if(buf[l] == 'p')
				{
					strcpy(level.spawnlist[k].name, "player_start");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 0;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == 'l')
				{
					strcpy(level.spawnlist[k].name, "longshot");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 1;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == 'f')
				{
					strcpy(level.spawnlist[k].name, "flyer");
					level.spawnlist[k].sx = i*40;
					level.spawnlist[k].sy = j*40;
					level.spawnlist[k].UnitType = 2;
					level.spawncount++;
					k++;
					i++;
				}
				else if(buf[l] == ';')
				{
					j++;
					break;
				}
			
			}
				i = 0;
				fgets(buf, sizeof(buf), file);
		}
		/*  for(j = 0;j < y;j++)
		{
		level.tilemap[j][0] = 1;
		level.tilemap[j][x - 1] = 1;    
		}
		for(i = 0;i < x;i++)
		{
		level.tilemap[0][i] = 1;
		level.tilemap[y - 1][i] = 1;    
		}*/
	}
	fclose(file);
}


/*load the graphics and draw the graphics to the background buffer*/
void DrawLevel()
{
	Sprite *tiles = NULL;
	SDL_Surface *temp;
	int i,j;
	/*first we load the sprite sheet for the background tiles*/
	switch(level.tileset)
	{
	case 1:
	default:
		tiles = LoadSprite("images/tiles/dirt.png",40,40);
		SDL_SetColorKey(tiles->image, SDL_SRCCOLORKEY , SDL_MapRGBA(tiles->image->format, 255,255,255,255));
	}
	if(tiles == NULL)
	{
		fprintf(stderr,"Unable to load tile set for map: %s \n",SDL_GetError());
		exit(0);
	}
	/*if(BgMusic != NULL)
	{
	Mix_HaltMusic();
	Mix_FreeMusic(BgMusic);
	}
	/*then we load the background music*/
	/*BgMusic = Mix_LoadMUS(level.bgmusic);
	if(BgMusic == NULL)
	{
	fprintf(stderr,"Unable to load music: %s \n",SDL_GetError());
	exit(0);
	}*/

	/*And we load the background image*/
	if(bgimage != NULL)SDL_FreeSurface(bgimage);
	temp = IMG_Load(level.bgimage);
	if(temp == NULL)
	{
		fprintf(stderr,"Unable to load background image: %s \n",SDL_GetError());
		exit(0);
	}
	bgimage = SDL_DisplayFormat(temp);
	SDL_SetColorKey(bgimage, SDL_RLEACCEL, 0);
	SDL_FreeSurface(temp);

	/*then we check to see if the background image is already in memory and if so, then we shall destroy it*/
	if(background != NULL)SDL_FreeSurface(background);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 40, level.height * 40, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	background = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);

	if(clipmask != NULL)SDL_FreeSurface(clipmask);
	temp = SDL_CreateRGBSurface(SDL_HWSURFACE, level.width * 40, level.height * 40, S_Data.depth,rmask, gmask,bmask,amask);
	if(temp == NULL)
	{
		fprintf(stderr,"Couldn't initialize background buffer: %s\n", SDL_GetError());
		exit(1);
	}
	/* Just to make sure that the surface we create is compatible with the screen*/
	clipmask = SDL_DisplayFormatAlpha(temp);
	SDL_FreeSurface(temp);
	// DrawFilledRect(0,0, background->w,background->h, SDL_MapRGB(background->format,0,0,0), background);
	for(j = 0;j < level.height;j++)
	{
		for(i = 0;i < level.width;i++)
		{
			DrawSprite(tiles,background,i * tiles->w,j * tiles->h,level.tilemap[j][i] - 1);
		}
	}
	SDL_BlitSurface(background,NULL,clipmask,NULL);
	SDL_SetColorKey(background,SDL_RLEACCEL, SDL_MapRGBA(background->format,0,0,0,255));
	FreeSprite(tiles);
	/*Mix_VolumeMusic(MIX_MAX_VOLUME>>3);
	Mix_PlayMusic(BgMusic, -1);*/
}

void SaveLevel(char *filename)
{
	FILE *file;
	file = fopen(filename, "wb");
	if(file == NULL)
	{
		fprintf(stderr,"Unable to open file for writing: %s",SDL_GetError());
		exit(0);
	}
	if(fwrite(&level, sizeof(level), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to write to file: %s",SDL_GetError());
		exit(0);
	}
	fclose(file);
}

void LoadLevel(char *filename)
{
	FILE *file;
	file = fopen(filename, "rb");
	if(file == NULL)
	{
		fprintf(stderr,"Unable to open file for reading: %s",SDL_GetError());
		exit(0);
	}
	if(fread(&level, sizeof(level), 1, file)!= 1)
	{
		fprintf(stderr,"Unable to read from file: %s",SDL_GetError());
		exit(0);    
	}
	fclose(file);
}

/*ye olde end of line at the end of file*/
