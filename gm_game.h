#ifndef GM_GAME_H
#define GM_GAME_H

#include "game.h"
#include "object.h"

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
	ObjectManager objman;
	u8 r, g, b;
} GM_Game_Work;

Gamemode GM_Game_Update(void *vwk);
void GM_Game_Render(void *vwk);

#endif
