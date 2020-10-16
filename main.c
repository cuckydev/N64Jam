/*
Project: N64Jam

File: main.c
Purpose: Provide the entry point

Authors: Regan "cuckydev" Green
*/

#include <nusys.h>

#include "mem.h"
#include "render.h"
#include "game.h"

void VBlankCallback(int pending)
{
	UpdateGame();
	if (pending < 1)
	{
		StartRenderFrame();
		RenderGame();
		EndRenderFrame();
	}
}

void mainproc(void)
{
	//Initialize memory heap
	static u8 mem_heap[0x80000];
	if (Mem_Init((void*)mem_heap, sizeof(mem_heap)))
		return;
	
	//Initialize sub-systems
	InitRender((NUGfxFunc)VBlankCallback);
	
	//Initialize game
	StartGame();
	
	//Start game
	StartRender();
	
	while (1);
}
