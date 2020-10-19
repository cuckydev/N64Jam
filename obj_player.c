#include <ultra64.h>

#include "obj_player.h"
#include "render.h"
#include "math_util.h"
#include "input.h"

//Player physics parameters
typedef struct
{
	f32 ground_accel;
	f32 ground_decel;
	f32 ground_drag;
	
	f32 air_accel;
	f32 air_drag;
	
	f32 jump_speed;
	f32 run_speed;
	
	f32 gravity;
	f32 hold_gravity;
	f32 gravity_drag;
} PlayerParam;

static const PlayerParam standard_param = {
	/* ground_accel */ 0.2f,
	/* ground_decel */ 0.005f,
	/* ground_drag */ 0.9f,
	
	/* air_accel */ 0.075f,
	/* air_drag */ 0.97f,
	
	/* jump_speed */ 4.75f,
	/* run_speed */ 1.25f,
	
	/* gravity */ 0.2f,
	/* hold_gravity */ 0.1f,
	/* gravity_drag */ 0.975f,
};

//Player movement functions
BOOL ObjPlayer_CheckJump(ObjPlayer_Work *wk, ObjectState *state, const PlayerParam *param)
{
	//Jump if A is pressed
	if (input_press & INPUT_A)
	{
		state->ysp = -param->jump_speed;
		wk->state = ObjPlayerState_Jump;
		wk->walk_iner = max(wk->walk_iner, 0x400);
		wk->collide &= ~OBJ_COLLIDE_FLOOR;
		return TRUE;
	}
	return FALSE;
}

//Object update
BOOL ObjPlayer_Update(Object *obj, ObjectManager *objman, Map *map)
{
	//Get object node information
	ObjPlayer_Work *wk = (ObjPlayer_Work*)obj->work;
	ObjectState *state = &obj->state;
	
	//Get physics parameters to use
	const PlayerParam *param = &standard_param;
	
	//Update player state
	switch (wk->state)
	{
		case ObjPlayerState_Idle:
			if (!(wk->collide & OBJ_COLLIDE_FLOOR))
			{
				//Walked off a ledge
				wk->state = ObjPlayerState_Jump;
			}
			else if (!ObjPlayer_CheckJump(wk, state, param))
			{
				//Didn't jump, switch to walk state if left or right is held
				if (input_down & (INPUT_LEFT | INPUT_RIGHT))
					wk->state = ObjPlayerState_Walk;
			}
			break;
		case ObjPlayerState_Walk:
			if (!(wk->collide & OBJ_COLLIDE_FLOOR))
			{
				//Walked off a ledge
				wk->state = ObjPlayerState_Jump;
			}
			else if (!ObjPlayer_CheckJump(wk, state, param))
			{
				//Didn't jump, switch to idle state if neither left or right is held
				if (!(input_down & (INPUT_LEFT | INPUT_RIGHT)))
					wk->state = ObjPlayerState_Idle;
			}
			break;
		case ObjPlayerState_Jump:
			if (wk->collide & OBJ_COLLIDE_FLOOR)
			{
				if (input_down & (INPUT_LEFT | INPUT_RIGHT))
					wk->state = ObjPlayerState_Walk;
				else
					wk->state = ObjPlayerState_Idle;
			}
			break;
	}
	
	//Size
	if (input_down & INPUT_UP)
		state->sx += 0.025f;
	if (input_down & INPUT_DOWN)
		state->sx -= 0.025f;
	state->sy = state->sx;
	
	//Run player state
	switch (wk->state)
	{
		case ObjPlayerState_Init:
		{
			//Initialize player state
			wk->state = ObjPlayerState_Idle;
			
			//Animation state
			wk->walk_tick = 0x0000;
			wk->walk_iner = 0x0000;
			wk->walk_per = 0.0f;
			
			//Collision state
			wk->collide = 0;
		}
	//Fallthrough
		case ObjPlayerState_Idle:
		{
			//Decrease animation weight
			wk->walk_per *= 0.9f;
			if (wk->walk_per < 0.01f)
				wk->walk_tick = 0x0000;
			
			//Decelerate
			state->xsp *= param->ground_drag;
			state->xsp = max(abs(state->xsp) - param->ground_decel, 0) * sign(state->xsp);
			break;
		}
		case ObjPlayerState_Walk:
		{
			//Increase animation weight
			f32 tgt_per = min(abs(state->xsp) / param->run_speed, 1.0f);
			if (wk->walk_per < tgt_per)
				wk->walk_per = min(wk->walk_per + 0.125f, tgt_per);
			else if (wk->walk_per > tgt_per)
				wk->walk_per = max(wk->walk_per - 0.1f, tgt_per);
			
			//Movement
			if (input_down & INPUT_LEFT)
				state->x_flip = TRUE;
			if (input_down & INPUT_RIGHT)
				state->x_flip = FALSE;
			
			//Accelerate
			state->xsp *= param->ground_drag;
			state->xsp += param->ground_accel * (state->x_flip ? -1.0f : 1.0f);
			break;
		}
		case ObjPlayerState_Jump:
		{
			//Increase animation weight
			if (wk->walk_per < 1.0f)
				wk->walk_per = min(wk->walk_per + 0.075f, 1.0f);
			
			//Movement
			state->xsp *= param->air_drag;
			if (input_down & INPUT_LEFT)
				state->xsp -= param->air_accel;
			if (input_down & INPUT_RIGHT)
				state->xsp += param->air_accel;
			
			//Gravity
			state->ysp *= param->gravity_drag;
			if ((input_down & INPUT_A) && state->ysp < 0.0f)
				state->ysp += param->hold_gravity;
			else
				state->ysp += param->gravity;
			break;
		}
	}
	
	//Move
	state->x += state->xsp;
	state->y += state->ysp;
	
	//Animation
	switch (wk->state)
	{
		case ObjPlayerState_Idle:
		case ObjPlayerState_Walk:
			wk->walk_iner = (abs(state->xsp) / param->run_speed) * 0x800;
			wk->walk_tick += wk->walk_iner;
			break;
		case ObjPlayerState_Jump:
			if ((wk->walk_tick >= 0x2000 && wk->walk_tick <= 0x6000) ||
				(wk->walk_tick >= 0xA000 && wk->walk_tick <= 0xE000))
				wk->walk_iner *= 0.6f;
			wk->walk_tick += wk->walk_iner;
			break;
	}
	
	//Collision detection
	wk->collide = ObjectManager_CollideMap(obj, 8.0f * state->sx, (((wk->collide & OBJ_COLLIDE_FLOOR) || state->ysp >= 0.0f) ? 16.0f : 14.0f) * state->sy, map);
	
	//Stop speeds if hit collision
	if ((wk->collide & OBJ_COLLIDE_LEFT) && state->xsp < 0.0f)
		state->xsp = 0.0f;
	if ((wk->collide & OBJ_COLLIDE_CEILING) && state->ysp < 0.0f)
		state->ysp = 0.0f;
	if ((wk->collide & OBJ_COLLIDE_RIGHT) && state->xsp > 0.0f)
		state->xsp = 0.0f;
	if ((wk->collide & OBJ_COLLIDE_FLOOR) && state->ysp > 0.0f)
		state->ysp = 0.0f;
	
	return FALSE;
}

//Object draw
#include "assets/player.inc.c"

static void Player_DrawPart(s32 bx, s32 by, f32 sx, f32 sy, BOOL x_flip, BOOL y_flip, f32 x, f32 y, u8 px, u8 py)
{
	//Get render state
	f32 fx = x_flip ? -1.0f : 1.0f;
	f32 fy = y_flip ? -1.0f : 1.0f;
	
	//Get render rects
	s32 nx = bx + (s32)((x * 16.0f) * (sx * fx));
	s32 ny = by + (s32)((y * 16.0f) * (sy * fy));
	s32 xrad = (s32)(4.0f * sx);
	s32 yrad = (s32)(4.0f * sy);
	
	Rect src = {px * 8, py * 8, px * 8, py * 8};
	if (x_flip)
		src.left += 8;
	else
		src.right += 8;
	if (y_flip)
		src.top += 8;
	else
		src.bottom += 8;
	Rect dst = {nx - xrad, ny - yrad, nx + xrad, ny + yrad};
	
	//Render part
	LoadTex_CI4(32, 32, player_limbs_tex, player_tlut);
	RenderTex(&src, &dst);
}

void ObjPlayer_Render(Object *obj, ObjectManager *objman)
{
	//Get object node information
	ObjPlayer_Work *wk = (ObjPlayer_Work*)obj->work;
	ObjectState *state = &obj->state;
	
	//Get animation state
	u8 walk_ang = wk->walk_tick >> 8;
	
	f32 bob = (-0.5f + lerp(1, coss(walk_ang << 1), wk->walk_per)) * 0.1f * state->sy;
	
	f32 hand_sin = lerp(0.0f, sins(walk_ang), wk->walk_per);
	f32 hand_cos = lerp(1.0f, -1.0f - coss(walk_ang), wk->walk_per);
	f32 foot_sin = lerp(0.0f, sins(walk_ang), wk->walk_per);
	f32 foot_cos = lerp(1.0f, coss(walk_ang << 1), wk->walk_per);
	
	f32 larm_x = -0.2f;
	f32 larm_y = 0.4f;
	f32 lhand_x = larm_x + (hand_sin * 0.4f);
	f32 lhand_y = (larm_y + 0.25f) + (hand_cos * 0.0825f);
	
	f32 rarm_x = 0.0f;
	f32 rarm_y = 0.4f;
	f32 rhand_x = rarm_x + (hand_sin * -0.4f);
	f32 rhand_y = (rarm_y + 0.25f) + (lerp(hand_cos, -1.0f - hand_cos, wk->walk_per) * 0.0825f);
	
	f32 lfoot_x = -0.3 + (foot_sin * -0.25f);
	f32 lfoot_y = min(0.9 + (foot_cos * 0.3f), 0.8);
	
	f32 rfoot_x = 0.1 + (foot_sin * 0.25f);
	f32 rfoot_y = min(0.9 + (foot_cos * 0.3f), 0.8);
	
	u8 lhand_frame = (hand_cos < -0.75f) ? 3 : 0;
	if (hand_sin < -0.5f)
		lhand_frame = 2;
	if (hand_sin > 0.5f)
		lhand_frame = 1;
	
	u8 rhand_frame = (hand_cos > -0.75f) ? 3 : 0;
	if (hand_sin < -0.5f)
		rhand_frame = 1;
	if (hand_sin > 0.5f)
		rhand_frame = 2;
	
	u8 lfoot_frame = 0;
	if (foot_sin < -0.5f)
		lfoot_frame = 1;
	if (foot_sin > 0.5f)
		lfoot_frame = 2;
	
	u8 rfoot_frame = 0;
	if (foot_sin < -0.5f)
		rfoot_frame = 2;
	if (foot_sin > 0.5f)
		rfoot_frame = 1;
	
	f32 foot_x, foot_y, foot_frame;
	
	//Get body rects
	s32 x = (s32)state->x;
	s32 y = (s32)(state->y + bob * state->sy);
	s32 xrad = (s32)(16.0f * state->sx);
	s32 yrad = (s32)(16.0f * state->sy);
	
	Rect body_src = {state->x_flip ? 32 : 0, 0, state->x_flip ? 0 : 32, 32};
	Rect body_dst = {x - xrad, y - yrad, x + xrad, y + yrad};
	
	//Render background limbs
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, rarm_x, rarm_y, 0, 0);
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, lerp(rarm_x, rhand_x, 0.35f), lerp(rarm_y, rhand_y, 0.35f), 0, 0);
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, rhand_x, rhand_y, 1, rhand_frame);
	foot_x = (foot_sin < 0) ? rfoot_x : lfoot_x;
	foot_y = (foot_sin < 0) ? rfoot_y : lfoot_y;
	foot_frame = (foot_sin < 0) ? rfoot_frame : lfoot_frame;
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, foot_x, foot_y, 3, foot_frame);
	
	//Render body
	LoadTex_CI4(32, 32, player_body_00_tex, player_tlut);
	RenderTex(&body_src, &body_dst);
	
	//Render foreground limbs
	foot_x = (foot_sin >= 0) ? rfoot_x : lfoot_x;
	foot_y = (foot_sin >= 0) ? rfoot_y : lfoot_y;
	foot_frame = (foot_sin >= 0) ? rfoot_frame : lfoot_frame;
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, foot_x, foot_y, 3, foot_frame);
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, larm_x, larm_y, 0, 0);
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, lerp(larm_x, lhand_x, 0.35f), lerp(larm_y, lhand_y, 0.35f), 0, 0);
	Player_DrawPart(x, y, state->sx, state->sy, state->x_flip, state->y_flip, lhand_x, lhand_y, 1, lhand_frame);
}
