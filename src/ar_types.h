#ifndef __AR_TYPES__
#define __AR_TYPES__

#include <SDL.h>


typedef Uint8	arUI8;
typedef Sint8	arSI8;
typedef Uint16	arUI16;
typedef Sint16	arSI16;
typedef Uint32	arUI32;
typedef Sint32	arSI32;
typedef	Uint64	arUI64;
typedef	Sint64	arSI64;

typedef	arSI32	arInt;
typedef arUI32	arUint;

typedef arUI8	arBool;
typedef SDL_Joystick arJoystick;

enum	arBoolEnum{
arTrue = 1,
arFalse = 0
};

/* This is the precursor for my event handling system */
enum SDLEventCategory
{
	EventCat_Keyboard,
	EventCat_Mouse,
	EventCat_System
};

typedef struct
{
	float x,y;
}vec2d;

typedef struct COORD_S
{
	float x, y;
}Coord;

typedef struct POINT_S
{
	int x, y;
}Point;

typedef SDL_Rect arRect;
typedef SDL_Surface arSurface;


/*
*@brief costructs and returns an arRect
*@param x component of the rect
*@param y component of the rect
*@param w component of the rect
*@param h component of the rect
*/

arRect ar_rect(
	arInt x,
	arInt y,
	arInt w,
	arInt h
);

/*
*@brief copies rect data from s into d
*
*@param d pointer to destination rect
*@param s source rect
*/
/*void rect_copy(
	arRect * d,
	arRect s
);

void rect_set(
	arRect * r,
	arInt	x,
	arInt	y,
	arInt	w,
	arInt	h
);*?

/*
*@brief checks if the two rects are overlapping
*@param a one of the rects to check
*@param b the other rect to check
*@return arTrue if they overlap, arFalse otherwise
*/

//arBool rect_olap_rect(arRect a, arRect b);


/*
*@brief check if the second rect is inside the first rect
*@param a outer rect
*@param b inner rect
*@return arTrue if b is inside a, arFale if any part of b is outside of a
*/

//arBool rect_in_rect(arRect a, arRect b);


#endif

