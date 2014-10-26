#include <string.h>
#include "spawn.h"
//#include "mechs.h"
//#include "worldents.h"
#include "player.h"
#include "space.h"
#include "entity.h"
#include "enemy.h"

extern arSurface *screen;
extern arRect Camera;
extern Level level;
extern Entity *ThePlayer;
int MaxSpawns;


Spawn GameSpawns[] = 
{
  {
    {8,12,36,26},             /*bounding box for wall detection*/
    "player_start",                 /*the name of the entity*/
    "images/playersprites/playersheet.png",      /*the sprite for the main part of the entity*/
    54,67,                    /*width and height of sprite dimensions*/
    /*{                         /*a list of pointers to the wav files that this entity will produce*/
      /*"\0",
      "\0",
      "\0",
      "\0"
    },*/                        
    NULL,                /*spawn function*/
   /* {
      0,0                     /*offset coordinates to draw the legs at
    },*/    
    NULL           
  },
 {
  {8,12,36,26},
  "longshot",
  "images/enemies/mecha1.png",
  48,48,
  {
	  "\0",
	  "\0",
	  "\0",
	  "\0"
  },
  SpawnenemyM,
  NULL,
},

 {
  {8,12,36,26},
  "flyer",
  "images/enemies/flyer.png",
  38,59,
  {
	  "\0",
	  "\0",
	  "\0",
	  "\0"
  },
  SpawnFlyer,
  NULL,
},
  {
    {0,0,0,0},             /*bounding box for wall detection*/
    "\0",                 /*the name of the entity*/
    "\0",      /*the sprite for the main part of the entity*/
    0,0,                    /*width and height of sprite dimensions*/
    /*{                         /*a list of pointers to the wav files that this entity will produce*/
      /*"\0",
      "\0",
      "\0",
      "\0"
    }, */                       
    NULL,                /*spawn function*/
    /*
      0,0                     /*offset coordinates to draw the legs at
    },*/
    NULL
  }
};

int GetSpawnIndexByName(char EntName[40])
{
  int i;
  for(i =0;i < MaxSpawns;i++)
  {
    if(strncmp(EntName,GameSpawns[i].EntName,40)== 0)return i;
  }
  return -1;/*not found*/
}

  /*only after a map's info has been loaded*/
void PrecacheSpawns()                       
{
  int i,j;
  i =0;
  for(i = 0;i < level.spawncount;i++)
  {
    for(j = 0;j < SOUNDSPERENT;i++)
    {
      if(GameSpawns[GetSpawnIndexByName(level.spawnlist[i].name)].sound[j][0] != '\0')
      {
        LoadSound(GameSpawns[GetSpawnIndexByName(level.spawnlist[i].name)].sound[j],SDL_MIX_MAXVOLUME>>4);
      }
    }
  }
}

void LoadSpawnSprites()
{
  int index;
  int i = 0;
  while(strncmp(GameSpawns[i].EntName,"\0",40 )!=0)
  {
    i++;
  }
  MaxSpawns = i;
  for(index = 0;index < MaxSpawns;index++)
  {
    GameSpawns[index].mapsprite = LoadSprite(GameSpawns[index].sprite,GameSpawns[index].sw,GameSpawns[index].sh);
  }
}


void DrawSpawnPoints()
{
  int i;
  for(i = 0;i < level.spawncount;i++)
  {
    DrawSpawn(GetSpawnIndexByName(level.spawnlist[i].name),level.spawnlist[i].sx - Camera.x,level.spawnlist[i].sy - Camera.y);    
  }
}

/*draws the desired spawn candidate at the location*/
void DrawSpawn(int index,int sx, int sy)     
{
  if(GameSpawns[index].mapsprite != NULL)
  DrawSprite(GameSpawns[index].mapsprite,screen,sx,sy,0);
  /*by not freeing the sprite, I ensure that it only gets loaded from disk once.*/
}

void SpawnAll(int initial)                               /*after map is loaded, start all entities*/
{
  int sindex;
  int i = 0;
  while(strncmp(GameSpawns[i].EntName,"\0",40 )!=0)
  {
    i++;
  }
  MaxSpawns = i;

  for(i = 0;i < level.spawncount;i++)
  {
    sindex = GetSpawnIndexByName(level.spawnlist[i].name);
    if(GameSpawns[sindex].spawn != NULL)
      GameSpawns[sindex].spawn(NULL,level.spawnlist[i].sx,level.spawnlist[i].sy,level.spawnlist[i].UnitInfo,level.spawnlist[i].UnitType);
    else
    {
      if(strncmp(level.spawnlist[i].name,"player_start",40) == 0)
      {
        if(initial == 1)
        {
          SpawnPlayer(level.spawnlist[i].sx,level.spawnlist[i].sy);
        }
        else
        {
          ThePlayer->s.x = level.spawnlist[i].sx;
          ThePlayer->s.y = level.spawnlist[i].sy;
          ThePlayer->v.x = 0;
          ThePlayer->v.y = 0;
          ThePlayer->a.x = 0;
          ThePlayer->a.y = 0;
          UpdateEntityPosition(ThePlayer,0);
        }
      }
      /*if(strncmp(level.spawnlist[i].name,"func_door",40) == 0)
      {
       SetDoor(level.spawnlist[i].sx, level.spawnlist[i].sy, 0,0,0);
      }*/
    }
  }
}