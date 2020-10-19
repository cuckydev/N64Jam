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

void ObjectManager_Update(ObjectManager *objman)
{
	//Update all object nodes
	for (Object *node = objman->head; node != NULL;)
	{
		Object *next = node->next;
		
		//Update object node
		if (node->update != NULL)
		{
			if (node->update(node, objman))
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
