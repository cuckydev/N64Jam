#ifndef GAME_H
#define GAME_H

#include "game_def.h"

//Game types
typedef enum
{
	GM_Null,
	GM_Game,
	GM_Num
} Gamemode;

typedef Gamemode (*GMFunc_Update)(void*);
typedef void (*GMFunc_Render)(void*);

//Game interface
void StartGame();
void UpdateGame();
void RenderGame();

#endif
