#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "object.h"

typedef enum
{
	ObjPlayerState_Init,
	ObjPlayerState_Idle,
	ObjPlayerState_Walk,
	ObjPlayerState_Jump,
} ObjPlayer_State;

typedef struct
{
	//Player state
	ObjPlayer_State state;
	
	//Animation state
	u16 walk_tick;
	u16 walk_iner;
	f32 walk_per;
	
	//Collision state
	ObjCollideField collide;
} ObjPlayer_Work;

BOOL ObjPlayer_Update(Object *obj, ObjectManager *objman, Map *map);
void ObjPlayer_Render(Object *obj, ObjectManager *objman);

#endif
