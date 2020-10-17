#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "object.h"

typedef enum
{
	ObjPlayerState_Init,
	ObjPlayerState_Idle,
	ObjPlayerState_Walk,
} ObjPlayer_State;

typedef struct
{
	//Player state
	ObjPlayer_State state;
	
	//Animation state
	u16 walk_tick;
	f32 walk_per;
} ObjPlayer_Work;

BOOL ObjPlayer_Update(void *vwk, ObjectState *state);
void ObjPlayer_Render(void *vwk, ObjectState *state);

#endif
