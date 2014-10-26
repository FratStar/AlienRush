#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "weapons.h"
#include "particle.h"
#include "projectiles.h"
                        /*2Pi*/
#define Sixteenth       0.392699081698724155
#define Twentyfifth     0.251327412287183459
#define Fiftieth        0.125663706143591729
#define Hundredth       0.062831853071795864
#define TwoHundredth    0.031415926535897932
#define ThreeHundredth  0.020943951023931955
#define FourHundredth   0.015707963267948966

Sound *WeaponSounds[NumWeapons * 2];

void GetSpawnByDir(Entity *self,int *sx,int *sy,int offset,int dir);

void FireShotgun(Entity *self,Weapon *weapon,int direction);
void UltimateShotgun(Entity *self,Weapon *weapon);
void FireMachinegun(Entity *self,Weapon *weapon,int direction);
void UltimateMachinegun(Entity *self,Weapon *weapon);
void FireRifle(Entity *self,Weapon *weapon,int direction);
void UltimateRifle(Entity *self,Weapon *weapon);
void FireLaser(Entity *self,Weapon *weapon,int direction);
void UltimateLaser(Entity *self,Weapon *weapon);
void FireRocket(Entity *self,Weapon *weapon,int direction);
void UltimateRocket(Entity *self,Weapon *weapon);
void FireMissile(Entity *self,Weapon *weapon,int direction);
void UltimateMissile(Entity *self,Weapon *weapon);
void FireGLauncher(Entity *self,Weapon *weapon,int direction);
void UltimateGLauncher(Entity *self,Weapon *weapon);
void FireCluster(Entity *self,Weapon *weapon,int direction);
void UltimateCluster(Entity *self,Weapon *weapon);
void FireFlameThrower(Entity *self,Weapon *weapon,int direction);
void UltimateFlameThrower(Entity *self,Weapon *weapon);
void FireSword(Entity *self,Weapon *weapon,int direction);
void UltimateSword(Entity *self,Weapon *weapon);

Weapon WeaponList[] = 
{
  {
    "Shotgun",          /*weapon's name*/
    8,                 /*how long it will take to cool down between shots*/
    26,                 /*Ultimate cooldown*/
    8,                  /*how much damage each projectile will deal*/
    0.2,                  /*how much kick the weapon carries*/
    16,                 /*speed*/
    0.2,                /*how much variation can be expeted between each bullet.*/
    6,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    B_Tiny,             /*some weapons have different sized projectiles*/
    FireShotgun,        /*call this when you fire the weapon*/
    "sounds/shotty.wav",
    "sounds/bigshotgun.wav"
  },
  {
    "Machine Gun",          /*weapon's name*/
    0,                 /*how long it will take to cool down between shots*/
    16,                 /*Ultimate cooldown*/
    25,                  /*how much damage each projectile will deal*/
    0.,                  /*how much kick the weapon carries*/
    18,                 /*speed*/
    0.15,                /*how much variation can be expeted between each bullet.*/
    3,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    B_Large,             /*some weapons have different sized projectiles*/
    FireMachinegun,        /*call this when you fire the weapon*/
    "sounds/machineg.wav",
    "\0"
  },
  {
    "Laser Rifle",          /*weapon's name*/
    4,                  /*how long it will take to cool down between shots*/
    24,                 /*Ultimate cooldown*/
    12,                  /*how much damage each projectile will deal*/
    0,                  /*how much kick the weapon carries*/
    18,                   /*speed*/
    0,                /*how much variation can be expeted between each bullet.*/
    1,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    1,                  /*bounce*/
    2,             /*some weapons have different sized projectiles*/
    FireLaser,        /*call this when you fire the weapon*/
    "sounds/medatck1.wav",
    "\0"
  },
  {
    "Fire Rocket",          /*weapon's name*/
    1,                  /*how long it will take to cool down between shots*/
    30,                 /*Ultimate cooldown*/
    50,                  /*how much damage each projectile will deal*/
    1,                  /*how much kick the weapon carries*/
    16,                   /*speed*/
    0,                /*how much variation can be expeted between each bullet.*/
    1,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    1,             /*some weapons have different sized projectiles*/
    FireRocket,        /*call this when you fire the weapon*/
    "sounds/rocketfly.wav",
    "\0"
  },
  {
    "Seeker Missile",          /*weapon's name*/
    0,                  /*how long it will take to cool down between shots*/
    30,                 /*Ultimate cooldown*/
    25,                  /*how much damage each projectile will deal*/
    1,                  /*how much kick the weapon carries*/
    20,                   /*speed*/
    0,                /*how much variation can be expeted between each bullet.*/
    3,                  /*depth of search fo a target, KEEP IT SMALL*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    1,             /*some weapons have different sized projectiles*/
    FireMissile,        /*call this when you fire the weapon*/
    "sounds/rocketfly.wav",
    "\0"
  },
  {
    "Flame Thrower",          /*weapon's name*/
    2,                  /*how long it will take to cool down between shots*/
    24,                 /*Ultimate cooldown*/
    7,                  /*how much damage each projectile will deal*/
    0.2,                  /*how much kick the weapon carries*/
    12,                   /*speed*/
    0.15,                /*how much variation can be expeted between each bullet.*/
    2,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    60,             /*some weapons have different sized projectiles*/
    FireFlameThrower,        /*call this when you fire the weapon*/
    "sounds/fthrower.wav",
    "\0"
  },
  /*This marks the beginning of non-player weapons*/
  {
    "Simple Bullet",          /*weapon's name*/
    5,                  /*how long it will take to cool down between shots*/
    24,                 /*Ultimate cooldown*/
    4,                  /*how much damage each projectile will deal*/
    0.5,                  /*how much kick the weapon carries*/
    6,                   /*speed*/
    0.01,                /*how much variation can be expeted between each bullet.*/
    1,                  /*volume of attack*/
    0,                  /*if the projectiles produced will be affected by gravity*/
    0,                  /*bounce*/
    B_Large,             /*some weapons have different sized projectiles*/
    FireRifle,        /*call this when you fire the weapon*/
    "sounds/rifleshot.wav",
    "sounds/xplode.wav"
  }
};


void FireWeapon(Entity *self,int Direction)
{
  if((self->currentweapon >= 0)&&(self->currentweapon < NumWeapons))
  {
    if(WeaponList[self->currentweapon].fireweapon != NULL)
    {
      WeaponList[self->currentweapon].fireweapon(self,&WeaponList[self->currentweapon],Direction);
      self->Cooldown = WeaponList[self->currentweapon].cooldown;
      if(WeaponSounds[self->currentweapon *2] != NULL)
      {
        Mix_PlayChannel(Mix_GroupAvailable(FX_Bullets),WeaponSounds[self->currentweapon *2]->sound,0);
      }
    }
  }
}    
 
/*************************************************************
              
                       Basic Shotgun

 *************************************************************/
void FireShotgun(Entity *self,Weapon *weapon,int direction)
{
  float angle;
  float dir = GetRadFromDir(direction);
  int i;
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  for(i = 0;i < weapon->volume;i++)
  {
    angle = dir + (weapon->spread * crandom());
    SpawnBullet(self,sx,sy,angle,weapon->speed * (fabs(crandom()) + 1) ,weapon->damage,weapon->kick, weapon->size, self->Color, self->Unit_Type);
  }
}



/*************************************************************
              
                       Basic Machine Gun

 *************************************************************/
void FireMachinegun(Entity *self,Weapon *weapon,int direction)
{
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  SpawnBullet(self,sx,sy,dir + crandom() * weapon->spread,weapon->speed ,weapon->damage,weapon->kick, weapon->size, self->Color, self->Unit_Type);
  SpawnFallingParticle(IndexColor(Gold),sx,sy,(int)crandom(),-5,24);
}

/*************************************************************
              
                       Basic Rifle

 *************************************************************/
void FireRifle(Entity *self,Weapon *weapon,int direction)
{
  float angle;
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  angle = dir + (weapon->spread * crandom()); /*accoutn for variation*/
  SpawnBullet(self,sx,sy,angle,weapon->speed,weapon->damage,weapon->kick, weapon->size, self->Color, self->Unit_Type);
}

/*************************************************************
              
                       Basic Laser Rifle

 *************************************************************/
void FireLaser(Entity *self,Weapon *weapon,int direction)
{
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  SpawnLaser(self,sx,sy,dir,weapon->speed,weapon->damage,weapon->kick, weapon->size, self->Color, self->Unit_Type,weapon->gravity);
}


/*************************************************************
              
                       Basic Rocket Launcher

 *************************************************************/
void FireRocket(Entity *self,Weapon *weapon,int direction)
{
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  SpawnRocket(self,sx,sy,dir,weapon->speed,1,weapon->damage,weapon->kick,self->Color,self->Unit_Type);
}


/*************************************************************
              
                       Basic Missile Launcher

 *************************************************************/
void FireMissile(Entity *self,Weapon *weapon,int direction)
{
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  SpawnMissile(self,sx,sy,dir,weapon->speed,0,weapon->damage,weapon->kick,self->Color,weapon->volume,self->Unit_Type);
}

/*************************************************************
              
                       Flame Thrower

 *************************************************************/
void FireFlameThrower(Entity *self,Weapon *weapon,int direction)
{
  float angle;
  float dir = GetRadFromDir(direction);
  int sx,sy;
  GetSpawnByDir(self,&sx,&sy,0,direction);
  angle = dir + (weapon->spread * crandom()); /*accoutn for variation*/
  SpawnFlame(self,sx - 24,sy - 24,angle,weapon->speed,weapon->damage,weapon->kick, weapon->size, self->Unit_Type);
}

/*************************************************************
              
                       Other Supported Functions

 *************************************************************/
char *PrintWeaponName(int index)
{
  return WeaponList[index].Name;
}

void PrecacheWeaponSounds()                    /*load sounds that the weapons use*/
{
  int i;
  for(i = 0;i < NumWeapons;i++)
  {
    if(WeaponList[i].firesound[0] != '\0')
    {
      WeaponSounds[i*2] = LoadSound(WeaponList[i].firesound,MIX_MAX_VOLUME);
    }  
    else WeaponSounds[i*2] = NULL;
    if(WeaponList[i].ultimatesound[0] != '\0')
    {
      WeaponSounds[i*2 + 1] = LoadSound(WeaponList[i].ultimatesound,MIX_MAX_VOLUME);
    }
    else WeaponSounds[i*2 + 1] = NULL;
  }
}

           /*return the index of the weapon in the list by the weapon's name*/
int GetWeaponByName(char name[40])
{
  int i;
  for(i = 0;i < NumWeapons;i++)
  {
    if(strncmp(name,WeaponList[i].Name,40)==0)return i;
  }
  return 0;
}

void GetSpawnByDir(Entity *self,int *sx,int *sy,int offset,int dir)
{
  int angle;
  *sx = (int)self->s.x + self->origin.x;
  *sy = (int)self->s.y + self->origin.y;
  angle = (int)(offset * 1.4);
  switch(dir)
  {
    case F_South:
      *sy += offset;
      return;
    case F_SW:
      *sx -= angle;
      *sy += angle;      
      return;
    case F_West:
      *sx -= offset;
      return;
    case F_NW:
      *sx -= angle;
      *sy -= angle;      
      return;
    case F_North:
      *sy -= offset;
      return;
    case F_NE:
      *sx += angle;
      *sy -= angle;      
      return;
    case F_East:
      *sx += offset;
      return;
    case F_SE:
      *sx += angle;
      *sy += angle;      
      return;
  }
}
/*EOL@EOF*/
