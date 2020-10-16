#ifndef GM_GAME_H
#define GM_GAME_H

#include "game.h"

//Gamemode declaration
typedef struct
{
	u32 mode;
	u8 r, g, b;
} GM_Game_Work;

Gamemode GM_Game_Update(void *vwk);
void GM_Game_Render(void *vwk);

#endif
