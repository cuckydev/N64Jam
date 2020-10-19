#ifndef OBJECT_H
#define OBJECT_H

#include "game_def.h"

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

typedef BOOL (*Object_Update)(struct Object*, ObjectManager*);
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

//Object interface
void ObjectManager_Init(ObjectManager *objman);
void ObjectManager_Quit(ObjectManager *objman);
void ObjectManager_Update(ObjectManager *objman);
void ObjectManager_Render(ObjectManager *objman);

void ObjectManager_Insert(ObjectManager *objman, ObjectId id, const ObjectState *state);

#endif
