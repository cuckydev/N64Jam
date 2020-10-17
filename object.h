#ifndef OBJECT_H
#define OBJECT_H

#include "game_def.h"

//Object types
typedef enum
{
	ObjectId_Player,
	ObjectId_Num,
} ObjectId;

typedef struct
{
	f32 x, y; //position
	f32 xsp, ysp; //speed
	f32 sx, sy; //scale
	BOOL x_flip, y_flip;
} ObjectState;

typedef BOOL (*Object_Update)(void*, ObjectState*);
typedef void (*Object_Render)(void*, ObjectState*);

typedef struct ObjectNode
{
	//Object work and state
	ObjectState state;
	void *work;
	
	//Object information
	Object_Update update;
	Object_Render render;
	
	//Linked list
	struct ObjectNode *prev, *next;
} ObjectNode;

typedef struct
{
	//Head object node
	ObjectNode *head;
} ObjectManager;

//Object interface
void ObjectManager_Init(ObjectManager *objman);
void ObjectManager_Quit(ObjectManager *objman);
void ObjectManager_Update(ObjectManager *objman);
void ObjectManager_Draw(ObjectManager *objman);

void ObjectManager_Insert(ObjectManager *objman, ObjectId id, const ObjectState *state);

#endif
