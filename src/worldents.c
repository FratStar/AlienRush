#include <string.h>
#include <stdlib.h>
#include "particle.h"
#include "entity.h"
#include "worldents.h"
#include "weapons.h"
//#include "mechs.h"

void GeneratorThink(entity *self);
void UpdateGenerator(entity *self);
void ExploderThink(entity *self);

/************************************************************
 *
 *                Mech generator spawns mechs.
 *
 ************************************************************/

void SpawnMechGenerator(entity *owner,int sx,int sy,int MechIndex,int UType)
{
  entity *newent = NULL;
  newent = newentity();
  if(newent == NULL)
  {
    fprintf(stderr,"Unable to generate generator; %s",SDL_GetError());
    return;
  }
  strcpy(newent->EntName,"Mech Generator\0");
  newent->sprite = LoadSprite("images/generator1.png",48,48);
  //newent->legs = LoadSprite("images/mecha1.png",48,48);
  SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));
  //SDL_SetColorKey(newent->legs->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->legs->image->format, 0,0,0));
  newent->sound[SF_DYING] = LoadSound("sounds/deadexplode.wav",SDL_MIX_MAXVOLUME>>1);
  newent->sound[SF_IMPACT] = LoadSound("sounds/generate.wav",SDL_MIX_MAXVOLUME>>4);
  newent->owner = owner;
  newent->update = UpdateGenerator;
  newent->think = GeneratorThink;
  newent->UpdateRate = 60;
  newent->ThinkRate = 45;
  newent->Unit_Type = UType;
  newent->frame = 0;
  newent->currentweapon = MechIndex;
  newent->fcount = 10;
  newent->Color = DarkRed;
  newent->frate = 1;
//  /*newent->legstate = -1;*/    /*needed if we don't have separate legs*/
  newent->state = ar_IDLE;
  newent->healthmax = 50;
  newent->health = 50;
  newent->takedamage = 1;
  newent->s.x = sx;
  newent->s.y = sy;
  newent->origin.x = 24;
  newent->origin.y = 40;
  newent->v.x = 0;
  newent->totaloffset = 6;
  newent->v.y = 0;
  newent->bBox.x = 8;
  newent->bBox.y = 4;
  newent->bBox.w = 32;
  newent->bBox.h = 40;  
  newent->m.x = (sx + newent->origin.x) >> 6;
  newent->m.y = (sy + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
}

void GeneratorThink(entity *self)
{
  switch(self->state)
  {
    case ar_IDLE:
      self->fcount--;
      if(self->fcount <= 0)
      {
        self->state = ar_ATTACK1;
      }
      break;
    case ar_ATTACK1:
      self->frame++;
      if(self->frame >= 24)
      {
        self->frame = 22;
     //   SpawnMech(self,self->s.x,self->s.y - 8,self->currentweapon,self->Unit_Type);
        if(OnScreen(self))Mix_PlayChannel(Mix_GroupAvailable(FX_Monsters),self->sound[SF_IMPACT]->sound,0);        
        self->state = ar_WSWITCH;
        self->fcount = 10;
      }
      break;
    case ar_WSWITCH:
      self->frame--;
      if(self->frame == 0)
      {
        self->state = ar_IDLE;
      }
      break;
  }
}

void UpdateGenerator(entity *self)
{
  switch(self->state)
  {
    case ar_ALERT:
      self->state = ar_IDLE;
      break;
    case ar_DIE:
      self->fcount = 10;
      self->state = ar_DEAD;
      Mix_PlayChannel(Mix_GroupAvailable(FX_Monsters),self->sound[SF_DYING]->sound,0);
      break;
    case ar_DEAD:
      self->fcount--;
      if(self->fcount <= 0)
      {
        Freeentity(self);
        return;
      }
      ExplodingParticle(self->s.x + self->origin.x,self->s.y + self->origin.y,crandom(),crandom());
      break;
  }
}

/************************************************************
 *
 *                Mech generator spawns mechs.
 *
 ************************************************************/

void SpawnExploder(entity *owner,int sx,int sy,int weapon,int UType)
{
  entity *newent = NULL;
  newent = newentity();
  if(newent == NULL)
  {
    fprintf(stderr,"Unable to generate exploder; %s",SDL_GetError());
    return;
  }
  strcpy(newent->EntName,"exploder\0");
  newent->sprite = LoadSprite("images/exploder.png",32,32);
  newent->sound[SF_DYING] = LoadSound("sounds/deadexplode.wav",SDL_MIX_MAXVOLUME>>1);
  newent->owner = owner;
  newent->update = UpdateGenerator;
  newent->think = ExploderThink;
  newent->UpdateRate = 60;
  newent->ThinkRate = 45;
  newent->Unit_Type = UType;
  newent->frame = 0;
  newent->fcount = 10;
  newent->Color = LightRed;
  newent->frate = 1;
 // /*newent->legstate = -1;*/    /*needed if we don't have separate legs*/
  newent->state = ar_IDLE;
  newent->currentweapon = weapon;
  newent->PowerLevel = 2;
  newent->healthmax = 50;
  newent->health = 50;
  newent->takedamage = 1;
  newent->s.x = sx;
  newent->s.y = sy;
  newent->origin.x = 16;
  newent->origin.y = 16;
  newent->totaloffset = 0;
  newent->v.x = 0;
  newent->v.y = 0;
  newent->bBox.x = 4;
  newent->bBox.y = 4;
  newent->bBox.w = 24;
  newent->bBox.h = 24;  
  newent->m.x = (sx + newent->origin.x) >> 6;
  newent->m.y = (sy + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
}

void ExploderThink(entity *self)
{
  switch(self->state)
  {
    case ar_IDLE:
      self->fcount--;
      if(self->fcount <= 0)
      {
        if(OnScreen(self))
        {
           self->state = ar_ATTACK1;
        }
        else self->fcount = 2;
      }
      break;
    case ar_ATTACK1:
      self->frame++;
      if(self->frame == 3)
      {
        FireUltimateWeapon(self);
        self->state = ar_WSWITCH;
        self->fcount = 10;
      }
      break;
    case ar_WSWITCH:
      self->frame++;
      if(self->frame >= 16)
      {
        self->frame = 0;
        self->state = ar_IDLE;
      }
      break;
  }
}



