#include "object.h"
#include "mem.h"
#include <string.h>

//Object definitions
#include "obj_player.h"

typedef struct
{
	size_t work_size;
	Object_Update update;
	Object_Render render;
} Object_Def;

static const Object_Def object_def[ObjectId_Num] = {
	/* ObjectId_Player */ {
		sizeof(ObjPlayer_Work),
		ObjPlayer_Update,
		ObjPlayer_Render,
	},
};

//Object map collision
ObjCollideField Object_CollideTile_Block(Object *obj, f32 xrad, f32 yrad, f32 tx, f32 ty)
{
	//Get edge positions
	f32 half = 0.5f * MAP_TILE_DIM;
	f32 left = tx - half;
	f32 top = ty - half;
	f32 right = tx + half;
	f32 bottom = ty + half;
	
	//Collide
	ObjCollideField collide = 0;
	
	//Check for wall collision if in x-range
	if (obj->state.y >= (top - yrad * 0.5f) && obj->state.y <= (bottom + yrad * 0.5f))
	{
		if (obj->state.x < tx)
		{
			if (obj->state.x >= left - xrad)
			{
				obj->state.x = left - xrad;
				collide |= OBJ_COLLIDE_RIGHT;
			}
		}
		else
		{
			if (obj->state.x <= right + xrad)
			{
				obj->state.x = right + xrad;
				collide |= OBJ_COLLIDE_LEFT;
			}
		}
	}
	
	//Check for floor and ceiling collision if in x-range
	if (obj->state.x >= (left - xrad * 0.5f) && obj->state.x <= (right + xrad * 0.5f))
	{
		if (obj->state.y < ty)
		{
			if (obj->state.y >= top - yrad)
			{
				obj->state.y = top - yrad;
				collide |= OBJ_COLLIDE_FLOOR;
			}
		}
		else
		{
			if (obj->state.y <= bottom + yrad)
			{
				obj->state.y = bottom + yrad;
				collide |= OBJ_COLLIDE_CEILING;
			}
		}
	}
	
	return collide;
}

ObjCollideField Object_CollideTile(Object *obj, f32 xrad, f32 yrad, s32 tx, s32 ty, u8 tv)
{
	//Get tile middle position
	f32 mtx = ((f32)tx + 0.5f) * MAP_TILE_DIM;
	f32 mty = ((f32)ty + 0.5f) * MAP_TILE_DIM;
	
	//Handle tile based off tile type
	switch (tv)
	{
		case 1:
			return Object_CollideTile_Block(obj, xrad, yrad, mtx, mty);
		default:
			return 0;
	}
}

ObjCollideField ObjectManager_CollideMap(Object *obj, f32 xrad, f32 yrad, Map *map)
{
	ObjCollideField collide = 0;
	
	//Get tiles to collide with (packed coordinates)
	s32 left = (s32)((obj->state.x - xrad) / MAP_TILE_DIM) >> 1;
	s32 top = (s32)((obj->state.y - yrad) / MAP_TILE_DIM);
	s32 right = (s32)((obj->state.x + xrad + MAP_TILE_DIM - 1) / MAP_TILE_DIM) >> 1;
	s32 bottom = (s32)((obj->state.y + yrad + MAP_TILE_DIM - 1) / MAP_TILE_DIM);
	
	//Clamp area to map area
	if (left < 0)
		left = 0;
	if (top < 0)
		left = 0;
	if (right > map->pitch - 1)
		right = map->pitch - 1;
	if (bottom < map->height)
		bottom = map->height;
	
	//Collide in each contained row
	u8 *mapp_row = map->colmap + (top * map->pitch) + left;
	for (s32 y = top; y <= bottom; y++)
	{
		//Get row pointer
		u8 *mapp = mapp_row;
		mapp_row += map->pitch;
		
		//Collide row
		for (s32 x = left; x <= right; x++)
		{
			//Unpack two tiles
			u8 ln = *mapp >> 4;
			u8 rn = *mapp & 0xF;
			mapp++;
			
			//Collide with tiles
			if (ln != 0)
				collide |= Object_CollideTile(obj, xrad, yrad, (x << 1), y, ln);
			if (rn != 0)
				collide |= Object_CollideTile(obj, xrad, yrad, (x << 1) + 1, y, rn);
		}
	}
	
	return collide;
}

//Object interface
void ObjectManager_Init(ObjectManager *objman)
{
	//Initialize object manager state
	objman->head = NULL;
}

void ObjectManager_Quit(ObjectManager *objman)
{
	//Release all object nodes contained by object manager
	if (objman->head != NULL)
	{
		for (Object *node = objman->head; node != NULL;)
		{
			Object *next = node->next;
			
			//Free node contents and node
			if (node->work != NULL)
				Mem_Free(node->work);
			Mem_Free(node);
			
			node = next;
		}
		objman->head = NULL;
	}
}

void ObjectManager_Update(ObjectManager *objman, Map *map)
{
	//Update all object nodes
	for (Object *node = objman->head; node != NULL;)
	{
		Object *next = node->next;
		
		//Update object node
		if (node->update != NULL)
		{
			if (node->update(node, objman, map))
			{
				//Unlink and release object node
				if (node->work != NULL)
					Mem_Free(node->work);
				if (node->prev != NULL)
					node->prev->next = node->next;
				if (node->next != NULL)
					node->next->prev = node->prev;
				if (node == objman->head)
					objman->head = NULL;
				Mem_Free(node);
			}
		}
		
		node = next;
	}
}

void ObjectManager_Render(ObjectManager *objman)
{
	//Render all object nodes
	for (Object *node = objman->head; node != NULL; node = node->next)
	{
		//Update object node
		if (node->render != NULL)
			node->render(node, objman);
	}
}

void ObjectManager_Insert(ObjectManager *objman, ObjectId id, const ObjectState *state)
{
	//Create new object node using object id's definition
	const Object_Def *def = &object_def[id];
	
	Object *node = (Object*)Mem_Alloc(sizeof(Object));
	if (def->work_size != 0)
		node->work = Mem_Alloc(def->work_size);
	else
		node->work = NULL;
	memcpy(&node->state, state, sizeof(ObjectState));
	node->update = def->update;
	node->render = def->render;
	
	//Push object node to linked list
	if (objman->head != NULL)
		objman->head->prev = node;
	objman->head = node;
}
