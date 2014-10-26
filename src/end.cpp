#include "end.h"
#include "sprite.h"
#include "graphics.h"



extern Entity ThePlayer;
extern int lvl;
nextlevel = 1;

void UpdateThing(Entity *self);

void SpawnThing(Entity *owner,int x,int y,int weapon,int affiliation)
{
  Entity *newent = NULL;
  newent = NewEntity();
  if(newent == NULL)
  {
    fprintf(stderr,"Unable to generate enemy entity; %s",SDL_GetError());
    return;
  }
  strcpy(newent->EntName,"longshot\0");
  newent->sprite = LoadSprite("images/gamesprites/blackbox.png",40,40);
 /* newent->legs = NULL;*/
  /*SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));*/
  newent->owner = owner;
  newent->update = UpdateThing;
  newent->UpdateRate = 45;
  newent->ThinkRate = 45;
  newent->Unit_Type = affiliation;
  newent->frame = 0;
  newent->fcount = 0;
  newent->Color = Cyan;
  newent->frate = 1;
 /* newent->legstate = -1;    /*needed if we don't have separate legs*/
  newent->state = ST_IDLE;
  newent->aimdir = F_South;
  newent->healthmax = 20;
  newent->health = 20;
  newent->takedamage = 1;
  newent->s.x = x;
  newent->s.y = y;
  newent->currentweapon = weapon;
  newent->origin.x = 26;
  newent->origin.y = 16;
  newent->v.x = 0;
  newent->v.y = 0;
  newent->maxspeed = 10;
  newent->movespeed = 0;
  newent->accel = 3;
  newent->enemy = 1;
  newent->totaloffset = 0;
  newent->Boundingbox.x = 8;
  newent->Boundingbox.y = 12;
  newent->Boundingbox.w = 36;
  newent->Boundingbox.h = 26;  
  newent->m.x = (x + newent->origin.x) >> 6;
  newent->m.y = (y + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
}

void UpdateThing(Entity *self)
{
	Entity *target;
	target = ThePlayer;
	if(CollisionDetect(self->owner, target) == 1)
	{
		nextlevel++;
		lvl++;
	}
}