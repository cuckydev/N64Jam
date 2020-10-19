#ifndef OBJECT_H
#define OBJECT_H

#include "game_def.h"
#include "map.h"

//Object constants
typedef u8 ObjCollideField;
#define OBJ_COLLIDE_LEFT    (1 << 0)
#define OBJ_COLLIDE_CEILING (1 << 1)
#define OBJ_COLLIDE_RIGHT   (1 << 2)
#define OBJ_COLLIDE_FLOOR   (1 << 3)

//Object types
typedef enum
{
	ObjectId_Player,
	ObjectId_Num,
} ObjectId;

struct Object;

typedef struct
{
	//Head object node
	struct Object *head;
} ObjectManager;

typedef struct
{
	f32 x, y; //position
	f32 xsp, ysp; //speed
	f32 sx, sy; //scale
	BOOL x_flip, y_flip;
} ObjectState;

typedef BOOL (*Object_Update)(struct Object*, ObjectManager*, Map*);
typedef void (*Object_Render)(struct Object*, ObjectManager*);

typedef struct Object
{
	//Object work and state
	ObjectState state;
	void *work;
	
	//Object information
	Object_Update update;
	Object_Render render;
	
	//Linked list
	struct Object *prev, *next;
} Object;

//Common object functions
ObjCollideField ObjectManager_CollideMap(Object *obj, f32 xrad, f32 yrad, Map *map);

//Object manager interface
void ObjectManager_Init(ObjectManager *objman);
void ObjectManager_Quit(ObjectManager *objman);
void ObjectManager_Update(ObjectManager *objman, Map *map);
void ObjectManager_Render(ObjectManager *objman);

void ObjectManager_Insert(ObjectManager *objman, ObjectId id, const ObjectState *state);

#endif
