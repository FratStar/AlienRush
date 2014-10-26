#ifndef __ENEMY__
#define __ENEMY__
#include "entity.h"
/*
 *    Prototpyes needed for spawning mechs, a generic enemy class
 */

#define ENEMYMAX 128

typedef struct
{
	char name[80];
	char spritepar[80];
	Sprite *sprite;
	int sx, sy;
	float radius;
	int sightdistance;
	int sightrange;
}Enemy; 
void SpawnenemyM(Entity *owner,int x,int y,int weapon,int affiliation);
void SpawnFlyer(Entity *owner,int x,int y,int weapon,int affiliation);
int GetInRange(Entity *self);   /*move to range of target, return 1 when in range*/
void LoadMonsterList();
Enemy GetEnemyByName(Entity *self, Entity *other);
void UpdateEnemy(Entity *self);
void EnemyThink(Entity *self);
void EnemyAlert(Entity *self, Entity*other);
void EnemyPath(Entity *self, Entity *target);


#endif
