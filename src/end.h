#ifndef __end__
#define __end__

#include "entity.h"

typedef struct 
{
	Coord s; /*location of this thing*/
	int hit; /*if you hit this*/
}Thing;

int nextlevel;
SpawnThing(Entity *owner,int x,int y,int weapon,int affiliation);

#endif