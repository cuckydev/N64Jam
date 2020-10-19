#ifndef GM_GAME_H
#define GM_GAME_H

#include "game.h"
#include "object.h"
#include "map.h"

//Gamemode declaration
typedef enum
{
	GM_Game_State_Init,
	GM_Game_State_Loop,
} GM_Game_State;

typedef struct
{
	//Gamemode state
	GM_Game_State state;
	
	//Game state
	Map map;
	ObjectManager objman;
} GM_Game_Work;

Gamemode GM_Game_Update(void *vwk);
void GM_Game_Render(void *vwk);

#endif
