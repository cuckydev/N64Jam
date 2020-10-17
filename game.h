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

typedef Gamemode (*Gamemode_Update)(void*);
typedef void (*Gamemode_Render)(void*);

//Game interface
void StartGame();
void UpdateGame();
void RenderGame();

#endif
