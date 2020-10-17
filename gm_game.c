#include "gm_game.h"
#include "render.h"

Gamemode GM_Game_Update(void *vwk)
{
	//Cast void work to gamemode work structure
	GM_Game_Work *wk = (GM_Game_Work*)vwk;
	
	//Update gamemode
	Gamemode next_gm = GM_Game;
	
	switch (wk->state)
	{
		case GM_Game_State_Init: //Initialization
			//Initialize gamemode state
			wk->state = GM_Game_State_Loop;
			
			//Initialize game state
			ObjectManager_Init(&wk->objman);
			
			static const ObjectState player_state = {160.0f, 120.0f, 0.0f, 0.0f, 1.0f, 1.0f};
			ObjectManager_Insert(&wk->objman, ObjectId_Player, &player_state);
	//Fallthrough
		case GM_Game_State_Loop:
			//Update game state
			ObjectManager_Update(&wk->objman);
			
			//Test
			wk->r++;
			wk->g += 2;
			wk->b += 3;
			break;
	}
	
	//Check if gamemode should be deinitialized
	if (next_gm != GM_Game)
	{
		//Deinitialize gamemode
		ObjectManager_Quit(&wk->objman);
	}
	return next_gm;
}

void GM_Game_Render(void *vwk)
{
	//Cast void work to gamemode work structure
	GM_Game_Work *wk = (GM_Game_Work*)vwk;
	
	//Render game
	RenderRect(&grect_full, RGB(wk->r, wk->g, wk->b));
	
	ObjectManager_Draw(&wk->objman);
}
