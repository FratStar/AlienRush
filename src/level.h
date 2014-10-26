#ifndef __LEVEL__
#define __LEVEL__

#define MAX_X	128
#define MAX_Y	72
#define IS_SET(a,b) (a & b)
#define SET_FLAG(a,b) (a |= b)
#define REMOVE_FLAG(a,b) (a &=~ b)
#define TILENUM(a) (a & 7)
#define TFLAG_Tile1 1
#define TFLAG_Tile2 2
#define TFLAG_Tile3 4
#define TFLAG_PASS 8
#define TFLAG_PSPAWN 16
#define TFLAG_ESPAWN 32


struct Tile{
	char info;
	char spawns;
};

struct Map{
	struct Tile Tiles[MAX_X][MAX_Y];
};


void CreateLevel(char *filename);
void LoadNewLevel(char *filename);


#endif
