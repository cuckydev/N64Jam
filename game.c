#include <ultra64.h>

#include "game.h"
#include "mem.h"

//Gamemode definitions
#include "gm_game.h"

typedef struct
{
	size_t work_size;
	GMFunc_Update update;
	GMFunc_Render render;
} GamemodeDef;

static const GamemodeDef gamemode_def[GM_Num] = {
	/* GM_Null */ {
		0,
		NULL,
		NULL
	},
	/* GM_Game */ {
		sizeof(GM_Game_Work),
		GM_Game_Update,
		GM_Game_Render
	}
};

//Internal gamemode interface
Gamemode gamemode;
void *gamemode_work;
GMFunc_Update gamemode_update;
GMFunc_Render gamemode_render;

void SetGamemode(Gamemode next_gamemode)
{
	//Make sure gamemode changed
	if (gamemode == next_gamemode)
		return;
	
	//Release previous gamemode
	if (gamemode_work != NULL)
		Mem_Free(gamemode_work);
	
	//Initialize new gamemode
	const GamemodeDef *def = &gamemode_def[next_gamemode];
	if (def->work_size != 0)
		gamemode_work = Mem_Alloc(def->work_size);
	else
		gamemode_work = NULL;
	gamemode_update = def->update;
	gamemode_render = def->render;
	gamemode = next_gamemode;
}

//Game interface
void StartGame()
{
	//Initialize game state
	gamemode = GM_Null;
	SetGamemode(GM_Game);
}

void UpdateGame()
{
	//Update gamemode
	while (gamemode_update != NULL)
	{
		Gamemode next = gamemode_update(gamemode_work);
		if (next != gamemode)
			SetGamemode(next);
		else
			break;
	}
}

void RenderGame()
{
	//Render gamemode
	if (gamemode_render != NULL)
		gamemode_render(gamemode_work);
}
