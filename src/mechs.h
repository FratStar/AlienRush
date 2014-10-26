#ifndef __ENEMIES__
#define __ENEMIES__
#include "entity.h"
/*
 *    Prototpyes needed for spawning mechs, a generic enemy class
 */
#define MONSTERMAX 128

typedef struct Monster_S
{
	char name[80];
	char sp[80];
	Sprite *sprite;
	int sx, sy;
	float radius;
	int damage;
	float movespeed;
	float attackspeed;
	int sightdistance;
	int sightrange;

}enemy;

void LoadEnemyList();


#endif
