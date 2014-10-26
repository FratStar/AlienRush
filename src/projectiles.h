#ifndef __PROJECTILE__
#define __PROJECTILE__
#include "entity.h"

enum Bullet_Size {B_Tiny,B_Small,B_Medium,B_Large,B_Huge,B_Atomic};
void PrecacheProjSounds();
Entity *SpawnGrenade(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType,int bounce,int gravity);
Entity *SpawnCluster(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType,int bounce,int gravity);
Entity *SpawnBullet(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType);
Entity *SpawnRocket(Entity *owner,int sx,int sy,float angle,float speed,float accel,int damage,float kick,int color,int UType);
Entity *SpawnMissile(Entity *owner,int sx,int sy,float angle,float speed,float accel,int damage,float kick,int color,int depth,int UType);
Entity *SpawnLaser(Entity *owner,int sx,int sy,float angle,float speed,int damage,float kick,int size,int color,int UType,int gravity);
Entity *SpawnFlame(Entity *owner,int sx,int sy,float angle,float speed,int damage, float kick, int length,int UType);

float GetRadFromDir(int dir);

Entity *GetTouchingEnt(Entity *self);

#endif
