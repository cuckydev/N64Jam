#include "gm_game.h"
#include "render.h"

Gamemode GM_Game_Update(void *vwk)
{
	//Cast void work to game work structure
	GM_Game_Work *wk = (GM_Game_Work*)vwk;
	
	//Update gamemode
	Gamemode next_gm = GM_Game;
	
	switch (wk->mode)
	{
		case 0: //Initialization
			wk->mode = 1;
			wk->r = 0;
			wk->g = 0;
			wk->b = 0;
		case 1:
			wk->r++;
			wk->g += 2;
			wk->b += 3;
			break;
	}
	
	return next_gm;
}

void GM_Game_Render(void *vwk)
{
	//Cast void work to game work structure
	GM_Game_Work *wk = (GM_Game_Work*)vwk;
	
	//Render gamemode
	RenderRect(&grect_full, RGB(wk->r, wk->g, wk->b));
}
