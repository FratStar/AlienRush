#ifndef __CAMERA__
#define __CAMERA__

#include "ar_types.h"


typedef struct Camera_S
{
	arUint	cameratype;
	arVec2d	bounds;
	float	position;
	void	*target;
	void (*update)(struct Camera_S *cam, void *data);

}Camera

void camera_config();

void camera_init();

void camera_update();

void camera_setup();

void camer_set_target(void *target);


#endif
