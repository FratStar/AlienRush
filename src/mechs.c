#include <string.h>
#include <stdlib.h>
#include "particle.h"
#include "weapons.h"
#include "mechs.h"

extern Entity *ThePlayer;

void UpdateMech(Entity *self);
void MechThink(Entity *self);

void SpawnMech(Entity *owner,int x,int y,int weapon,int affiliation)
{
  Entity *newent = NULL;
  newent = NewEntity();
  if(newent == NULL)
  {
    fprintf(stderr,"Unable to generate mech entity; %s",SDL_GetError());
    return;
  }
  strcpy(newent->EntName,"Mech\0");
  newent->sprite = LoadSprite("images/mecha1.png",48,48);
 /* newent->legs = NULL;*/
  /*SDL_SetColorKey(newent->sprite->image, SDL_SRCCOLORKEY , SDL_MapRGB(newent->sprite->image->format, 0,0,0));*/
  newent->owner = owner;
  newent->update = UpdateMech;
  newent->think = MechThink;
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
  newent->totaloffset = 0;
  newent->Boundingbox.x = 8;
  newent->Boundingbox.y = 12;
  newent->Boundingbox.w = 36;
  newent->Boundingbox.h = 26;  
  newent->m.x = (x + newent->origin.x) >> 6;
  newent->m.y = (y + newent->origin.y) >> 6;
  AddEntToRegion(newent,newent->m.x,newent->m.y);
}

void MechThink(Entity *self)
{
  switch(self->state)
  {
    case ST_WALK:
      self->ordercount--;
      if(self->ordercount <= 0)self->state = ST_IDLE;
      break;
    case ST_IDLE:
      if(DistanceBetween(self, ThePlayer) < 150)
      {
        self->state = ST_ALERT;
      }
      else
      {
        self->state = ST_WALK;
        if(self->fcount == 0)
        {
          self->v.x = 4;
          self->fcount = 1;
        }
        else
        {
          self->v.x = -4;
          self->fcount = 0;
        }
        self->ordercount = 50;
      }
    break;
    case ST_ALERT:
      if((self->Cooldown <= 0)&&(DistanceBetween(self, ThePlayer) < 120))
      {
        self->aimdir = AimAtTarget(self,ThePlayer);
        self->state = ST_ATTACK1;
        return;
      }
      if(self->s.x < ThePlayer->s.x - 32)
      {
        self->v.x = 4;
      }
      else if(self->s.x > ThePlayer->s.x +32)
      {
        self->v.x = -4;
      }
      else self->v.x = 0;
      if(self->s.y < ThePlayer->s.y - 32)
      {
        self->v.y = 4;
        self->grounded = 0;
      }
      else if(self->s.y > ThePlayer->s.y + 32)
      {
        self->v.y = -4;
        self->grounded = 0;
      }
      else self->v.y = 0;
    break;
  }
}

void UpdateMech(Entity *self)
{
  UpdateEntityPosition(self,0);
  if(!self->grounded)
  {
    if(self->v.y < 20)
    self->v.y += 2;
  }
  if(self->Cooldown > 0)self->Cooldown--;
  switch(self->state)
  {
    case ST_IDLE:
      ApplyFriction(self,0.02);
      self->frame++;
        if(self->frame >= 2)
        {
          self->frame = 0;
        }
      break;
    case ST_ATTACK1:
      ApplyFriction(self,0.02);
      FireWeapon(self,self->aimdir);
      self->state = ST_ALERT;
      break;
    
    case ST_ALERT:
      ApplyFriction(self,0.02);
    case ST_WALK:
      GetFace(self);
      switch(self->face)
      {
        case F_North:
        case F_South:
          self->frame++;
          if((self->frame < 27)||(self->frame > 29))self->frame = 27;
        break;
        case F_West:
          if(self->grounded)
          {
            self->frame++;
            if((self->frame < 33)||(self->frame > 39))self->frame = 33;            
            break;
          }
        case F_SW:
        case F_NW:
          self->frame++;
          if((self->frame < 24)||(self->frame > 26))self->frame = 24;
          break;
        case F_East:
          if(self->grounded)
          {
            self->frame--;
            if((self->frame < 40)||(self->frame > 46))self->frame = 46;            
            break;
          }
        case F_SE:
        case F_NE:
          self->frame++;
          if((self->frame < 30)||(self->frame > 32))self->frame = 30;
          break;
      }
      break;
    case ST_DIE:
      self->fcount = 10;
      self->state = ST_DEAD;
      break;
    case ST_DEAD:
      self->fcount--;
      if(self->fcount <= 0)
      {
        FreeEntity(self);
        return;
      }
      ExplodingParticle(self->s.x + self->origin.x,self->s.y + self->origin.y,crandom(),crandom());
    default:
      return;
  }
}


/*end of line at eof*/
